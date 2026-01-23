#pragma once

#include "volk/volk.h"
#include "vara/renderer/vulkan_device.h"

typedef struct VulkanRendererState {
    VaraWindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VulkanDevice device;
} VulkanRendererState;