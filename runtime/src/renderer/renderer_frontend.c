#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"

typedef struct RendererState RendererState;

struct RendererState {
    RendererBackend* backend;
    RenderCommandBuffer* frame_cmd_buffer;
    VaraWindow* main_window;
};

static RendererState* renderer_state;

b8 renderer_create(VaraWindow* window) {
    if (renderer_state) {
        ERROR("Renderer('%s') is already initialized!", renderer_state->backend->name);
        return false;
    }

    renderer_state = platform_allocate(sizeof(RendererState));
    platform_zero_memory(renderer_state, sizeof(RendererState));
    if (!renderer_state) {
        FATAL("Failed to allocate RendererState.");
        return false;
    }

    renderer_state->main_window = window;
    renderer_state->backend = renderer_backend_create(window);
    renderer_backend_set(renderer_state->backend);

    if (!renderer_state->backend) {
        FATAL("Failed to create RendererBackend.");
        platform_free(renderer_state);
        return false;
    }

    renderer_state->frame_cmd_buffer = render_cmd_buffer_create();
    if (!renderer_state->frame_cmd_buffer) {
        FATAL("Failed to create the frames RenderCommandBuffer.");
        platform_free(renderer_state);
        return false;
    }

    return true;
}

void renderer_destroy(void) {
    if (renderer_state) {
        if (renderer_state->frame_cmd_buffer) {
            render_cmd_buffer_destroy(renderer_state->frame_cmd_buffer);
        }

        if (renderer_state->backend) {
            renderer_backend_destroy(renderer_state->backend);
        }

        platform_free(renderer_state);
    }
}

void renderer_on_window_resize(Vector2i new_size) {
    if (renderer_state) {
        const RendererBackend* backend = renderer_state->backend;
        if (backend->renderer.set_viewport) {
            backend->renderer.set_viewport(new_size);
        }
    }
}

PlatformRendererType renderer_get_renderer_type(void) {
    if (renderer_state) {
        return renderer_state->backend->type;
    }

    return RENDERER_TYPE_NONE;
}

RenderCommandBuffer* renderer_get_frame_command_buffer(void) {
    if (!renderer_state->frame_cmd_buffer) {
        renderer_state->frame_cmd_buffer = render_cmd_buffer_create();
    }
    return renderer_state->frame_cmd_buffer;
}

void renderer_begin_frame(void) {
    render_cmd_buffer_reset(renderer_get_frame_command_buffer());
}

void renderer_end_frame(void) {
    renderer_execute_commands(renderer_get_frame_command_buffer());
    renderer_present();
}

void renderer_clear(void) {
    if (renderer_state) {
        const RendererBackend* backend = renderer_state->backend;
        backend->renderer.clear();
    }
}

void renderer_clear_color(const Vector4 color) {
    if (renderer_state) {
        const RendererBackend* backend = renderer_state->backend;
        backend->renderer.clear_color(color);
    }
}

void renderer_execute_commands(RenderCommandBuffer* buffer) {
    if (renderer_state) {
        const RendererBackend* backend = renderer_state->backend;
        backend->renderer.submit(buffer);
    }
}

void renderer_present(void) {
    if (renderer_state) {
        const RendererBackend* backend = renderer_state->backend;
        backend->renderer.present();
    }
}
