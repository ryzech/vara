#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/swapchain.h"
#include "vara/renderer/renderer.h"

Swapchain* swapchain_create(Renderer* renderer, const SwapchainConfig* config) {
    Swapchain* swapchain = platform_allocate(sizeof(Swapchain));
    platform_zero_memory(swapchain, sizeof(Swapchain));

    swapchain->window = config->window;
    swapchain->image_count = config->image_count;
    swapchain->vsync = config->vsync;

    RendererBackend* backend = renderer_backend_get(renderer);
    swapchain->backend = backend;

    if (!swapchain->backend->swapchain.create(swapchain, config)) {
        ERROR("Failed to create Swapchain");
        swapchain_destroy(swapchain);
        return NULL;
    }

    return swapchain;
}

void swapchain_destroy(Swapchain* swapchain) {
    if (swapchain) {
        swapchain->backend->swapchain.destroy(swapchain);
        platform_free(swapchain);
    }
}

void swapchain_present(Swapchain* swapchain) {
    if (swapchain) {
        swapchain->backend->swapchain.present(swapchain);
    }
}