#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/swapchain.h>

#include "volk/volk.h"

typedef struct VulkanSwapchainState {
    VkSwapchainKHR swapchain;
} VulkanSwapchainState;

b8 swapchain_vulkan_create(Swapchain* swapchain, const SwapchainConfig* config);
void swapchain_vulkan_destroy(Swapchain* swapchain);
void swapchain_vulkan_present(Swapchain* swapchain);