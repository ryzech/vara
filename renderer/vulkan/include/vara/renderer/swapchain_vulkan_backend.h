#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/swapchain.h>

#include "vara/renderer/vulkan_common.h"

// Make this a configurable value.
#define MAX_FRAMES_IN_FLIGHT 3

typedef struct VulkanSwapchainState VulkanSwapchainState;
typedef struct VulkanFrame VulkanFrame;

struct VulkanFrame {
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkSemaphore image_available;
    VkSemaphore render_finished;
    VkFence in_flight;
};

struct VulkanSwapchainState {
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR image_format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent;
    u32 image_count;
    VkImage* images;
    VkImageView* views;
    VulkanFrame frames[MAX_FRAMES_IN_FLIGHT];
    u32 current_frame;
    u32 image_index;
    VkFence* images_in_flight;
};

b8 swapchain_vulkan_create(Swapchain* swapchain, const SwapchainConfig* config);
void swapchain_vulkan_destroy(Swapchain* swapchain);
void swapchain_vulkan_present(Swapchain* swapchain);

void swapchain_vulkan_begin_frame(Swapchain* swapchain);