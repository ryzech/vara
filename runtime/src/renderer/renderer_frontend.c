#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer2d/renderer2d.h"

Renderer* renderer_create(VaraWindow* window) {
    Renderer* renderer = platform_allocate(sizeof(Renderer));
    platform_zero_memory(renderer, sizeof(Renderer));
    if (!renderer) {
        FATAL("Failed to allocate Renderer.");
        return NULL;
    }

    renderer->window = window;
    renderer->backend = renderer_backend_create(window);

    if (!renderer->backend) {
        FATAL("Failed to create RendererBackend.");
        renderer_destroy(renderer);
        return NULL;
    }

    renderer->command_buffer = render_cmd_buffer_create();
    if (!renderer->command_buffer) {
        FATAL("Failed to create the frames RenderCommandBuffer.");
        renderer_destroy(renderer);
        return NULL;
    }

    const Renderer2DConfig renderer_2d_config = {
        .max_vertices = 4096,
        .max_indices = 4096,
    };
    if (!renderer2d_create(renderer, &renderer_2d_config)) {
        FATAL("Failed to create Renderer2D!");
        renderer_destroy(renderer);
        return NULL;
    }

    return renderer;
}

void renderer_destroy(Renderer* renderer) {
    if (renderer) {
        renderer2d_destroy();

        if (renderer->command_buffer) {
            render_cmd_buffer_destroy(renderer->command_buffer);
        }

        if (renderer->backend) {
            renderer_backend_destroy(renderer->backend);
        }

        platform_free(renderer);
    }
}

void renderer_on_window_resize(Renderer* renderer, Vector2i new_size) {
    if (renderer) {
        const RendererBackend* backend = renderer->backend;
        if (backend->renderer.set_viewport) {
            backend->renderer.set_viewport(new_size);
        }
    }
}

PlatformRendererType renderer_get_renderer_type(Renderer* renderer) {
    if (renderer) {
        return renderer->backend->type;
    }

    return RENDERER_TYPE_NONE;
}

RenderCommandBuffer* renderer_get_frame_command_buffer(Renderer* renderer) {
    if (!renderer->command_buffer) {
        renderer->command_buffer = render_cmd_buffer_create();
    }

    return renderer->command_buffer;
}

void renderer_begin_frame(Renderer* renderer) {
    render_cmd_buffer_reset(renderer_get_frame_command_buffer(renderer));
    renderer2d_begin(renderer);
}

void renderer_end_frame(Renderer* renderer) {
    renderer2d_end();
    renderer_execute_commands(renderer, renderer_get_frame_command_buffer(renderer));
    renderer_present(renderer);
}

void renderer_clear(Renderer* renderer) {
    if (renderer) {
        const RendererBackend* backend = renderer->backend;
        backend->renderer.clear();
    }
}

void renderer_clear_color(Renderer* renderer, const Vector4 color) {
    if (renderer) {
        const RendererBackend* backend = renderer->backend;
        backend->renderer.clear_color(color);
    }
}

void renderer_execute_commands(Renderer* renderer, RenderCommandBuffer* buffer) {
    if (renderer) {
        const RendererBackend* backend = renderer->backend;
        backend->renderer.submit(buffer);
    }
}

void renderer_present(Renderer* renderer) {
    if (renderer) {
        const RendererBackend* backend = renderer->backend;
        backend->renderer.present();
    }
}
