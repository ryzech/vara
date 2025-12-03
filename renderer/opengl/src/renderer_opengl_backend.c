#include <glad/gl.h>
#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/buffer_opengl_backend.h"
#include "vara/renderer/framebuffer_opengl_backend.h"
#include "vara/renderer/render_pass_opengl_backend.h"
#include "vara/renderer/shader_opengl_backend.h"
#include "vara/renderer/texture_opengl_backend.h"

typedef struct OpenGLRendererState {
    VaraWindow* window;
} OpenGLRendererState;

static OpenGLRendererState renderer_state;

static b8 renderer_opengl_create(void) {
    gladLoadGL((GLADloadfunc)platform_window_get_proc_address);
    glEnable(GL_DEBUG_OUTPUT);
    // Should likely move this into config somewhere, or be pass specific?
    glEnable(GL_DEPTH_TEST);
    DEBUG("Loaded OpenGL: %s | %s", glGetString(GL_VERSION), glGetString(GL_RENDERER));
    return true;
}

// Do we need glClear and glCLearColor? Should they be differentiated?
static void renderer_opengl_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Do we need glClear and glCLearColor? Should they be differentiated?
static void renderer_opengl_clear_color(Vector4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

static void renderer_opengl_set_viewport(Vector2i viewport_size) {
    // Calculate scaled size (framebuffer size).
    // Should be the same regardless of window, so we query the main window.
    const f32 scale = renderer_state.window->pixel_density;
    const i32 scaled_x = (i32)((f32)viewport_size.x * scale);
    const i32 scaled_y = (i32)((f32)viewport_size.y * scale);

    glViewport(0, 0, scaled_x, scaled_y);
}

static void renderer_opengl_submit(const RenderCommandBuffer* buffer) {
    u8* cmd = buffer->buffer;
    const u8* end = buffer->buffer + buffer->used;

    while (cmd < end) {
        const RenderCommandHeader* header = (RenderCommandHeader*)cmd;

        switch (header->type) {
            case RENDER_CMD_BEGIN_PASS: {
                const RenderCmdBeginPass* begin_pass = (RenderCmdBeginPass*)cmd;
                render_pass_opengl_begin(begin_pass->pass);
                break;
            }
            case RENDER_CMD_END_PASS: {
                const RenderCmdEndPass* end_pass = (RenderCmdEndPass*)cmd;
                render_pass_opengl_end(end_pass->pass);
                break;
            }
            case RENDER_CMD_DRAW_INDEXED: {
                const RenderCmdDrawIndexed* draw_indexed = (RenderCmdDrawIndexed*)cmd;
                // Doesn't feel like we should be calling these apis here.
                shader_bind(draw_indexed->shader);
                buffer_bind(draw_indexed->vertex);
                buffer_bind(draw_indexed->index);
                glDrawElements(
                    GL_TRIANGLES, (GLsizei)draw_indexed->index->element_count, GL_UNSIGNED_INT, NULL
                );
                break;
            }
            case RENDER_CMD_SET_SHADER_MAT4: {
                const RenderCmdSetShaderMat4* set_mat4 = (RenderCmdSetShaderMat4*)cmd;
                shader_bind(set_mat4->shader);
                shader_set_mat4(set_mat4->shader, set_mat4->name, set_mat4->matrix);
                break;
            }
            case RENDER_CMD_SET_SHADER_INT_ARRAY: {
                const RenderCmdSetShaderIntArray* set_int_array = (RenderCmdSetShaderIntArray*)cmd;
                shader_bind(set_int_array->shader);
                shader_set_int_array(
                    set_int_array->shader,
                    set_int_array->name,
                    set_int_array->array,
                    set_int_array->count
                );
                break;
            }
            default: {
                ERROR("Unknown render command type!");
                break;
            }
        }

        cmd += header->size;
    }
}

static void renderer_opengl_present(void) {
    platform_window_swap_buffers(renderer_state.window);
}

static void renderer_opengl_destroy(void) {
}

void renderer_opengl_init(RendererBackend* backend, VaraWindow* window) {
    renderer_state.window = window;

    backend->name = "OpenGL";
    backend->type = RENDERER_TYPE_OPENGL;

    // Core Renderer
    backend->renderer.create = renderer_opengl_create;
    backend->renderer.clear = renderer_opengl_clear;
    backend->renderer.clear_color = renderer_opengl_clear_color;
    backend->renderer.set_viewport = renderer_opengl_set_viewport;
    backend->renderer.submit = renderer_opengl_submit;
    backend->renderer.present = renderer_opengl_present;
    backend->renderer.destroy = renderer_opengl_destroy;

    // Buffer API
    backend->buffer.create = buffer_opengl_create;
    backend->buffer.destroy = buffer_opengl_destroy;
    backend->buffer.bind = buffer_opengl_bind;
    backend->buffer.unbind = buffer_opengl_unbind;
    backend->buffer.set_data = buffer_opengl_set_data;

    // Render Pass API
    backend->render_pass.create = render_pass_opengl_create;
    backend->render_pass.destroy = render_pass_opengl_destroy;
    backend->render_pass.begin = render_pass_opengl_begin;
    backend->render_pass.end = render_pass_opengl_end;

    // Shader API
    backend->shader.create = shader_opengl_create;
    backend->shader.destroy = shader_opengl_destroy;
    backend->shader.bind = shader_opengl_bind;
    backend->shader.unbind = shader_opengl_unbind;
    backend->shader.set_int_array = shader_opengl_set_int_array;
    backend->shader.set_mat4 = shader_opengl_set_mat4;
    backend->shader.dispatch = shader_opengl_dispatch;

    // Texture API
    backend->texture.create = texture_opengl_create;
    backend->texture.destroy = texture_opengl_destroy;
    backend->texture.bind = texture_opengl_bind;
    backend->texture.unbind = texture_opengl_unbind;
    backend->texture.set_data = texture_opengl_set_data;
    backend->texture.get_id = texture_opengl_get_id;

    // Framebuffer API
    backend->framebuffer.create = framebuffer_opengl_create;
    backend->framebuffer.destroy = framebuffer_opengl_destroy;
    backend->framebuffer.bind = framebuffer_opengl_bind;
    backend->framebuffer.unbind = framebuffer_opengl_unbind;
    backend->framebuffer.resize = framebuffer_opengl_resize;

    DEBUG("Creating RendererBackend named('%s')", backend->name);
}
