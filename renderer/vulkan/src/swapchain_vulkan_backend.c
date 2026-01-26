#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>

#include <sys/_types/_dev_t.h>
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/swapchain_vulkan_backend.h"

static VkSurfaceFormatKHR vulkan_choose_surface_format(VkSurfaceFormatKHR* formats) {
    for (u32 i = 0; i < array_length(formats); i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM
            && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }

    return formats[0];
}

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

    VkSurfaceFormatKHR surface_format =
        vulkan_choose_surface_format(renderer->device.surface_info.formats);

    return true;
}

void swapchain_vulkan_destroy(Swapchain* swapchain) {
}

void swapchain_vulkan_present(Swapchain* swapchain) {
    if (!swapchain || !swapchain->backend_data) {
        return;
    }

    VulkanSwapchainState* state = swapchain->backend_data;
    VulkanRendererState* renderer = swapchain->backend->backend_data;
}
