#include <vara/core/logger.h>
#include <vara/core/util/array.h>

#include "vara/renderer/vulkan_device.h"

static i32 vulkan_device_score(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);
    i32 score = 0;

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    score += properties.limits.maxImageDimension2D;

    return score;
}

b8 vulkan_device_create(VkInstance instance, VulkanDevice* device) {
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (device_count == 0) {
        FATAL("No GPUs supporting Vulkan found!");
        return false;
    }

    VkPhysicalDevice* devices = array_sized(device_count, VkPhysicalDevice, NULL);
    vkEnumeratePhysicalDevices(instance, &device_count, devices);
    array_set_length(devices, device_count);

    i32 last_score = 0;
    VkPhysicalDevice selected = VK_NULL_HANDLE;
    DEBUG("Found %u Vulkan device(s):", device_count);
    for (u32 i = 0; i < array_length(devices); i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        DEBUG("\t- %s", properties.deviceName);

        const i32 score = vulkan_device_score(devices[i]);
        if (score > last_score) {
            last_score = score;
            selected = devices[i];
        }
    }
    array_destroy(devices);

    if (selected == VK_NULL_HANDLE) {
        FATAL("Failed to find a supported GPU!");
        return false;
    }

    device->physical_device = selected;
    vkGetPhysicalDeviceProperties(selected, &device->properties);
    vkGetPhysicalDeviceFeatures(selected, &device->features);
    DEBUG("Using GPU named('%s')", device->properties.deviceName);

    return true;
}
