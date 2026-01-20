#pragma once

#include <vara/core/defines.h>

#include "volk/volk.h"

typedef struct VulkanDevice VulkanDevice;

struct VulkanDevice {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
};

b8 vulkan_device_create(VkInstance instance, VulkanDevice* device);