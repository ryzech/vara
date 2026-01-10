#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer/swapchain.h"

Renderer* renderer_create(VaraWindow* window) {
    Renderer* renderer = vara_allocate(sizeof(Renderer));
    vara_zero_memory(renderer, sizeof(Renderer));
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

    const SwapchainConfig swapchain_config = {
        .window = renderer->window,
        // Needs to be configurable somehow.
        .vsync = true,
        .image_count = 3,
    };
    renderer->swapchain = swapchain_create(renderer, &swapchain_config);
    if (!renderer->swapchain) {
        FATAL("Failed to create the windows Swapchain.");
        renderer_destroy(renderer);
        return NULL;
    }

    return renderer;
}

void renderer_destroy(Renderer* renderer) {
    if (renderer) {
        if (renderer->command_buffer) {
            render_cmd_buffer_destroy(renderer->command_buffer);
        }

        if (renderer->swapchain) {
            swapchain_destroy(renderer->swapchain);
        }

        if (renderer->backend) {
            renderer_backend_destroy(renderer->backend);
        }

        vara_free(renderer, sizeof(Renderer));
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

void renderer_begin_frame(Renderer* renderer) {
    if (renderer) {
        render_cmd_buffer_reset(renderer_get_frame_command_buffer(renderer));
    }
}

void renderer_end_frame(Renderer* renderer) {
    if (renderer) {
        renderer_execute_commands(renderer, renderer_get_frame_command_buffer(renderer));
        renderer_present(renderer);
    }
}

void renderer_present(Renderer* renderer) {
    if (renderer) {
        swapchain_present(renderer->swapchain);
    }
}

void renderer_execute_commands(Renderer* renderer, RenderCommandBuffer* buffer) {
    if (renderer) {
        const RendererBackend* backend = renderer->backend;
        backend->renderer.submit(buffer);
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