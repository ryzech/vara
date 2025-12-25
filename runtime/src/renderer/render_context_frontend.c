#include <vara/core/platform/platform.h>

#include "vara/renderer/render_context.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer2d/renderer2d.h"

RenderContext* render_context_create(Renderer* renderer) {
    RenderContext* context = platform_allocate(sizeof(RenderContext));
    platform_zero_memory(context, sizeof(RenderContext));

    if (!context) {
        return NULL;
    }

    const Renderer2DConfig r2d_config = {
        .max_indices = 4096,
        .max_vertices = 4096,
    };
    context->r2d = renderer2d_create(renderer, &r2d_config);

    return context;
}

void render_context_destroy(RenderContext* context) {
    if (context) {
        if (context->r2d) {
            renderer2d_destroy(context->r2d);
        }
        platform_free(context);
    }
}

void render_context_begin_frame(RenderContext* context) {
    if (context) {
        if (context->r2d) {
            renderer2d_begin(context->r2d);
        }
    }
}

void render_context_end_frame(RenderContext* context) {
    if (context) {
        if (context->r2d) {
            renderer2d_end(context->r2d);
        }
    }
}