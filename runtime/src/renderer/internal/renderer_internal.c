#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/renderer.h"

extern void renderer_opengl_init(RendererBackend* backend, VaraWindow* window);

RendererBackend* renderer_backend_create(VaraWindow* window) {
    RendererBackend* backend = platform_allocate(sizeof(RendererBackend));
    platform_zero_memory(backend, sizeof(RendererBackend));

    switch (window->renderer_type) {
        case RENDERER_TYPE_OPENGL:
            renderer_opengl_init(backend, window);
            break;
        default:
            ERROR("Unsupported renderer type.")
            break;
    }

    if (!backend->renderer.create()) {
        FATAL("Failed to initialize Renderer!");
        renderer_backend_destroy(backend);
        platform_free(backend);
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
