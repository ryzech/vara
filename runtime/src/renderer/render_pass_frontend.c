#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/framebuffer.h"
#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/renderer.h"

RenderPass* render_pass_create(const RenderPassConfig* config) {
    RenderPass* pass = platform_allocate(sizeof(RenderPass));
    platform_zero_memory(pass, sizeof(RenderPass));

    pass->name = config->name;
    pass->target = config->target;
    pass->clear = config->clear;
    pass->clear_color = config->clear_color;

    const RendererBackend* backend = renderer_backend_get();
    if (!backend->render_pass.create(pass, config)) {
        ERROR("Failed to create render pass named('%s')", config->name);
        render_pass_destroy(pass);
        return NULL;
    }

    return pass;
}

void render_pass_destroy(RenderPass* pass) {
    if (pass) {
        const RendererBackend* backend = renderer_backend_get();
        backend->render_pass.destroy(pass);
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
