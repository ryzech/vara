#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/renderer.h"

extern RendererInstance* renderer_opengl_init(VaraWindow* window);

RendererInstance* renderer_create(VaraWindow* window) {
    RendererInstance* instance = NULL;
    if (window) {
        switch (window->graphics_type) {
            case GRAPHICS_TYPE_OPENGL:
                instance = renderer_opengl_init(window);
                if (instance) {
                    platform_window_make_context_current(window);
                    if (!instance->vt.renderer_create()) {
                        platform_free(instance);
                        return NULL;
                    }
                }
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(window->graphics_type)
                );
                return NULL;
        }
    }

    return instance;
}

void renderer_destroy(RendererInstance* instance) {
    if (instance && instance->vt.renderer_destroy) {
        instance->vt.renderer_destroy();
    }
}

void renderer_clear(RendererInstance* instance) {
    if (instance && instance->vt.renderer_clear) {
        instance->vt.renderer_clear();
    }
}

void renderer_clear_color(RendererInstance* instance, Vector4 color) {
    if (instance && instance->vt.renderer_clear_color) {
        instance->vt.renderer_clear_color(color);
    }
}

void renderer_present(RendererInstance* instance) {
    if (instance && instance->vt.renderer_present) {
        instance->vt.renderer_present();
    }
}
