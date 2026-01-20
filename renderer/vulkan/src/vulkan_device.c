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

    VkPhysicalDeviceDriverProperties driver = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES
    };
    VkPhysicalDeviceProperties2 properties2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = &driver,
    };

    device->physical_device = selected;
    vkGetPhysicalDeviceProperties(selected, &device->properties);
    vkGetPhysicalDeviceProperties2(selected, &properties2);
    vkGetPhysicalDeviceFeatures(selected, &device->features);
    vkGetPhysicalDeviceMemoryProperties(selected, &device->memory);
    DEBUG("Using GPU named('%s'):", device->properties.deviceName);
    DEBUG("\tDriver: %s (%s)", driver.driverName, driver.driverInfo);

    const VkPhysicalDeviceFeatures features = {
        .samplerAnisotropy = VK_TRUE,
    };

    u32 extension_index = 0;
    const char* extensions[6] = {0};
    extensions[extension_index] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    extension_index++;
#if defined(VARA_PLATFORM_APPLE)
    extensions[extension_index] = "VK_KHR_portability_subset";
    extension_index++;
#endif

    const VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pEnabledFeatures = &features,
        .enabledExtensionCount = extension_index,
        .ppEnabledExtensionNames = extensions,
    };
    vkCreateDevice(device->physical_device, &info, NULL, &device->logical_device);
    volkLoadDevice(device->logical_device);
    const u32 major = VK_VERSION_MAJOR(device->properties.apiVersion);
    const u32 minor = VK_VERSION_MINOR(device->properties.apiVersion);
    const u32 patch = VK_VERSION_PATCH(device->properties.apiVersion);
    DEBUG("Using Vulkan API %u.%u.%u", major, minor, patch);

    return true;
}
