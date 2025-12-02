#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/framebuffer.h"
#include "vara/renderer/render_command.h"
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

    const RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case RENDERER_TYPE_OPENGL:
                render_pass_opengl_init(pass);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    renderer_type_to_string(instance->renderer_type)
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
    if (pass->target) {
        framebuffer_bind(pass->target);
    }
    if (pass->clear) {
        renderer_clear_color(pass->clear_color);
        renderer_clear();
    }
    render_cmd_begin_pass(renderer_get_frame_command_buffer(), pass);
}

void render_pass_end(RenderPass* pass) {
    render_cmd_end_pass(renderer_get_frame_command_buffer(), pass);
    if (pass->target) {
        framebuffer_unbind(pass->target);
    }
}
