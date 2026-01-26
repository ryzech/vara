#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/swapchain.h>

#include "volk/volk.h"

typedef struct VulkanSwapchainState {
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR image_format;
    u8 max_frames_in_flight;
    u32 image_count;
    VkImage* images;
    VkImageView* views;
} VulkanSwapchainState;

b8 swapchain_vulkan_create(Swapchain* swapchain, const SwapchainConfig* config);
void swapchain_vulkan_destroy(Swapchain* swapchain);
void swapchain_vulkan_present(Swapchain* swapchain);