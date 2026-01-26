#pragma once

#include <vara/core/defines.h>

#include "volk/volk.h"

typedef struct VulkanDevice VulkanDevice;
typedef struct VulkanSurfaceInfo VulkanSurfaceInfo;

struct VulkanSurfaceInfo {
    VkSurfaceFormatKHR* formats;
    VkPresentModeKHR* present_modes;
};

struct VulkanDevice {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    VulkanSurfaceInfo surface_info;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkQueue compute_queue;
    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;
    i32 compute_queue_index;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
};

b8 vulkan_device_create(VkInstance instance, VkSurfaceKHR surface, VulkanDevice* device);
void vulkan_device_destroy(VulkanDevice* device);