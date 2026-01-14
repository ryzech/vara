#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/renderer.h"

extern void renderer_opengl_init(RendererBackend* backend, VaraWindow* window);
extern void renderer_vulkan_init(RendererBackend* backend, VaraWindow* window);

RendererBackend* renderer_backend_create(VaraWindow* window) {
    RendererBackend* backend = vara_allocate(sizeof(RendererBackend));
    vara_zero_memory(backend, sizeof(RendererBackend));

    switch (window->renderer_type) {
        case RENDERER_TYPE_OPENGL:
            renderer_opengl_init(backend, window);
            break;
        case RENDERER_TYPE_VULKAN:
            renderer_vulkan_init(backend, window);
            break;
        default:
            ERROR("Unsupported renderer type.")
            break;
    }

    if (!backend->renderer.create()) {
        FATAL("Failed to initialize Renderer!");
        renderer_backend_destroy(backend);
        vara_free(backend, sizeof(RendererBackend));
        return NULL;
    }

    return backend;
}

void renderer_backend_destroy(RendererBackend* backend) {
    if (backend) {
        backend->renderer.destroy();
    }
}

RendererBackend* renderer_backend_get(Renderer* renderer) {
    if (renderer->backend) {
        return renderer->backend;
    }

    return NULL;
}
