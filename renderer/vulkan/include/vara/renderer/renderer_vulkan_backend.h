#pragma once

#include "vara/renderer/vulkan_device.h"
#include "volk/volk.h"

// Make this a configurable value.
#define MAX_FRAMES_IN_FLIGHT 2

typedef struct VulkanRendererState VulkanRendererState;
typedef struct VulkanFrame VulkanFrame;

struct VulkanFrame {
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkSemaphore image_available;
    VkSemaphore render_finished;
    VkFence in_flight;
};

struct VulkanRendererState {
    VaraWindow* window;
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    VulkanDevice device;
    VulkanFrame frames[MAX_FRAMES_IN_FLIGHT];
    u32 current_frame;
    u32 image_index;
};