#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/renderer.h"

extern RendererInstance* renderer_opengl_init(VaraWindow* window);

static RendererInstance* instance;

b8 renderer_create(VaraWindow* window) {
    instance = NULL;
    // Support headless with a Renderer?
    if (window) {
        switch (window->graphics_type) {
            case GRAPHICS_TYPE_OPENGL:
                instance = renderer_opengl_init(window);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(window->graphics_type)
                );
                return false;
        }
    }

    if (!instance) {
        return false;
    }

    platform_window_make_context_current(window);
    if (!instance->vt.renderer_create()) {
        platform_free(instance);
        return false;
    }

    return true;
}

void renderer_destroy(void) {
    if (instance && instance->vt.renderer_destroy) {
        instance->vt.renderer_destroy();
    }
}

RendererInstance* renderer_get_instance(void) {
    if (instance) {
        return instance;
    }

    return NULL;
}

PlatformGraphicsType renderer_get_renderer_type(void) {
    if (instance) {
        return instance->renderer_type;
    }

    return GRAPHICS_TYPE_NONE;
}

void renderer_clear(void) {
    if (instance && instance->vt.renderer_clear) {
        instance->vt.renderer_clear();
    }
}

void renderer_clear_color(Vector4 color) {
    if (instance && instance->vt.renderer_clear_color) {
        instance->vt.renderer_clear_color(color);
    }
}

void renderer_set_viewport(Vector2i position, Vector2i size) {
    if (instance && instance->vt.renderer_set_viewport) {
        instance->vt.renderer_set_viewport(position, size);
    }
}

void renderer_present(void) {
    if (instance && instance->vt.renderer_present) {
        instance->vt.renderer_present();
    }
}
