#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"

extern void renderer_opengl_init(RendererInstance* instance, VaraWindow* window);

static RendererInstance* instance;
static RenderCommandBuffer* command_buffer;

b8 renderer_create(VaraWindow* window) {
    instance = platform_allocate(sizeof(RendererInstance));
    platform_zero_memory(instance, sizeof(RendererInstance));

    instance->renderer_type = window->renderer_type;

    // Support headless with a Renderer?
    switch (window->renderer_type) {
        case RENDERER_TYPE_OPENGL:
            renderer_opengl_init(instance, window);
            break;
        default:
            ERROR("Unsupported graphics type: %s", renderer_type_to_string(window->renderer_type));
            return false;
    }

    platform_window_make_context_current(window);
    if (!instance->vt.renderer_create()) {
        renderer_destroy();
        return false;
    }

    return true;
}

void renderer_destroy(void) {
    if (instance && instance->vt.renderer_destroy) {
        instance->vt.renderer_destroy();
        platform_free(instance);
    }
    if (command_buffer) {
        render_cmd_buffer_destroy(command_buffer);
    }
}

void renderer_on_window_resize(Vector2i new_size) {
    render_cmd_set_viewport(renderer_get_frame_command_buffer(), new_size);
}

RendererInstance* renderer_get_instance(void) {
    if (instance) {
        return instance;
    }

    return NULL;
}

PlatformRendererType renderer_get_renderer_type(void) {
    if (instance) {
        return instance->renderer_type;
    }

    return RENDERER_TYPE_NONE;
}

RenderCommandBuffer* renderer_get_frame_command_buffer(void) {
    if (!command_buffer) {
        command_buffer = render_cmd_buffer_create();
    }
    return command_buffer;
}

void renderer_begin_frame(void) {
    render_cmd_buffer_reset(renderer_get_frame_command_buffer());
}

void renderer_end_frame(void) {
    renderer_execute_commands(renderer_get_frame_command_buffer());
    renderer_present();
}

void renderer_clear(void) {
    if (instance && instance->vt.renderer_clear) {
        instance->vt.renderer_clear();
    }
}

void renderer_clear_color(const Vector4 color) {
    if (instance && instance->vt.renderer_clear_color) {
        instance->vt.renderer_clear_color(color);
    }
}

void renderer_execute_commands(RenderCommandBuffer* buffer) {
    if (instance && instance->vt.renderer_execute_commands) {
        instance->vt.renderer_execute_commands(buffer);
    }
}

void renderer_present(void) {
    if (instance && instance->vt.renderer_present) {
        instance->vt.renderer_present();
    }
}
