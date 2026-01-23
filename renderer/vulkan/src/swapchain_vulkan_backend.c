#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/swapchain_vulkan_backend.h"

b8 swapchain_vulkan_create(Swapchain* swapchain, const SwapchainConfig* config) {
    if (!swapchain || !config) {
        return false;
    }

    DEBUG("Creating Swapchain for VaraWindow named('%s')", config->window->name);
    VulkanSwapchainState* state = vara_allocate(sizeof(VulkanSwapchainState));
    vara_zero_memory(state, sizeof(VulkanSwapchainState));
    if (!state) {
        return false;
    }

    VulkanRendererState* renderer = swapchain->backend->backend_data;

    swapchain->backend_data = state;

    return true;
}

void swapchain_vulkan_destroy(Swapchain* swapchain) {
}

void swapchain_vulkan_present(Swapchain* swapchain) {
}
