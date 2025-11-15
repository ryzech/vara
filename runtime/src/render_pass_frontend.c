#include "vara/renderer/buffer.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/renderer.h"

#include <stdlib.h>

#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

extern RenderPass* render_pass_opengl_init(const RenderPassConfig* config);

RenderPass* render_pass_create(
    RendererInstance* instance, const RenderPassConfig* config
) {
    RenderPass* render_pass = NULL;
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
        render_pass->vt.render_pass_begin(render_pass);
    }
}

void render_pass_draw_indexed(
    RenderPass* pass, struct Buffer* vertex_buffer, struct Buffer* index_buffer
) {
    if (pass && pass->vt.render_pass_draw_indexed) {
        buffer_bind(vertex_buffer);
        buffer_bind(index_buffer);
        pass->vt.render_pass_draw_indexed(pass, index_buffer);
    }
}

void render_pass_end(RenderPass* render_pass) {
    if (render_pass && render_pass->vt.render_pass_end) {
        render_pass->vt.render_pass_end(render_pass);
    }
}
