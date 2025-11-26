#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/renderer.h"

extern void render_pass_opengl_init(RenderPass* pass);

RenderPass* render_pass_create(const RenderPassConfig* config) {
    RenderPass* pass = platform_allocate(sizeof(RenderPass));
    platform_zero_memory(pass, sizeof(RenderPass));

    pass->name = config->name;
    pass->target = config->target;
    pass->clear = config->clear;
    pass->clear_color = config->clear_color;
    // Set false on creation, only true when pass begins.
    pass->is_recording = false;

    const RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                render_pass_opengl_init(pass);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(instance->renderer_type)
                );
                return NULL;
        }
    }

    if (!pass->vt.render_pass_create(pass, config)) {
        ERROR("Failed to create render pass named('%s')", config->name);
        render_pass_destroy(pass);
        return NULL;
    }

    return pass;
}

void render_pass_destroy(RenderPass* pass) {
    if (pass && pass->vt.render_pass_destroy) {
        pass->vt.render_pass_destroy(pass);
        platform_free(pass);
    }
}

void render_pass_begin(RenderPass* pass) {
    if (pass && pass->vt.render_pass_begin) {
        if (pass->target) {
            framebuffer_bind(pass->target);
        }
        if (pass->clear) {
            renderer_clear_color(pass->clear_color);
            renderer_clear();
        }
        pass->vt.render_pass_begin(pass);
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

void render_pass_end(RenderPass* pass) {
    if (pass && pass->vt.render_pass_end) {
        pass->is_recording = false;

        if (pass->target) {
            framebuffer_unbind(pass->target);
        }
        pass->vt.render_pass_end(pass);
    }
}
