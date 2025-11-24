#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/renderer.h"

extern RenderPass* render_pass_opengl_init(const RenderPassConfig* config);

RenderPass* render_pass_create(const RenderPassConfig* config) {
    RenderPass* render_pass = NULL;
    RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                render_pass = render_pass_opengl_init(config);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(instance->renderer_type)
                );
                return NULL;
        }
    }

    if (!render_pass) {
        return NULL;
    }

    render_pass->name = config->name;

    if (!render_pass->vt.render_pass_create(render_pass, config)) {
        ERROR("Failed to create render pass named('%s')", config->name);
        platform_free(render_pass);
        return NULL;
    }

    return render_pass;
}

void render_pass_destroy(RenderPass* render_pass) {
    if (render_pass && render_pass->vt.render_pass_destroy) {
        render_pass->vt.render_pass_destroy(render_pass);
        platform_free(render_pass);
    }
}

void render_pass_begin(RenderPass* render_pass) {
    if (render_pass && render_pass->vt.render_pass_begin) {
        if (render_pass->config->target) {
            framebuffer_bind(render_pass->config->target);
        }
        render_pass->vt.render_pass_begin(render_pass);
    }
}

void render_pass_draw_indexed(
    RenderPass* pass,
    Shader* shader,
    Buffer* vertex_buffer,
    Buffer* index_buffer
) {
    if (pass && pass->vt.render_pass_draw_indexed) {
        shader_bind(shader);
        buffer_bind(vertex_buffer);
        buffer_bind(index_buffer);
        pass->vt.render_pass_draw_indexed(pass, index_buffer);
    }
}

void render_pass_end(RenderPass* render_pass) {
    if (render_pass && render_pass->vt.render_pass_end) {
        if (render_pass->config->target) {
            framebuffer_unbind(render_pass->config->target);
        }
        render_pass->vt.render_pass_end(render_pass);
    }
}
