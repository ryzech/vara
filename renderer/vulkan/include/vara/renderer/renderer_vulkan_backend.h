#pragma once

#include "swapchain_vulkan_backend.h"
#include "vara/renderer/vulkan_device.h"
#include "volk/volk.h"

typedef struct VulkanRendererState VulkanRendererState;

struct VulkanRendererState {
    VaraWindow* window;
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    VulkanDevice device;
    Swapchain* swapchain;
};