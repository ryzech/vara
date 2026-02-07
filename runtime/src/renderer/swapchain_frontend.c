#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer/swapchain.h"

Swapchain* swapchain_create(Renderer* renderer, const SwapchainConfig* config) {
    Swapchain* swapchain = vara_allocate(sizeof(Swapchain));
    vara_zero_memory(swapchain, sizeof(Swapchain));

    swapchain->window = config->window;
    swapchain->vsync = config->vsync;

    RendererBackend* backend = renderer_backend_get(renderer);
    swapchain->backend = backend;

    if (swapchain->backend->swapchain.create) {
        if (!swapchain->backend->swapchain.create(swapchain, config)) {
            ERROR("Failed to create Swapchain");
            swapchain_destroy(swapchain);
            return NULL;
        }
    }

    return swapchain;
}

void swapchain_destroy(Swapchain* swapchain) {
    if (swapchain) {
        if (swapchain->backend->swapchain.destroy) {
            swapchain->backend->swapchain.destroy(swapchain);
        }
        vara_free(swapchain, sizeof(Swapchain));
    }
}

void swapchain_present(Swapchain* swapchain) {
    if (swapchain) {
        if (swapchain->backend->swapchain.present) {
            swapchain->backend->swapchain.present(swapchain);
        }
    }
}

RenderTarget* swapchain_get_current_target(Swapchain* swapchain) {
    if (swapchain) {
        if (swapchain->backend->swapchain.get_current_target) {
            return swapchain->backend->swapchain.get_current_target(swapchain);
        }
    }

    return NULL;
}