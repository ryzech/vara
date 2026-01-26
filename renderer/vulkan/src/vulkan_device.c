#include <vara/core/logger.h>
#include <vara/core/util/array.h>
#include <vara/core/util/string.h>

#include "vara/renderer/vulkan_device.h"

typedef struct QueueFamilyIndices {
    i32 graphics;
    i32 present;
    i32 transfer;
    i32 compute;
} QueueFamilyIndices;

static b8 vulkan_device_supports_extensions(VkPhysicalDevice device, const char** required) {
    u32 available_count = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &available_count, NULL);
    VkExtensionProperties* available = array_sized(available_count, VkExtensionProperties, NULL);
    vkEnumerateDeviceExtensionProperties(device, NULL, &available_count, available);
    array_set_length(available, available_count);

    for (u32 i = 0; i < array_length(required); i++) {
        b8 found = false;
        for (u32 j = 0; j < array_length(available); j++) {
            if (strings_equal(required[i], available[j].extensionName)) {
                found = true;
                break;
            }
        }
        if (!found) {
            array_destroy(available);
            return false;
        }
    }

    array_destroy(available);
    return true;
}

static QueueFamilyIndices vulkan_device_find_queue_families(
    VkPhysicalDevice device, VkSurfaceKHR surface
) {
    QueueFamilyIndices indices = {
        .graphics = -1,
        .present = -1,
        .transfer = -1,
        .compute = -1,
    };

    u32 property_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &property_count, NULL);
    VkQueueFamilyProperties* properties =
        array_sized(property_count, VkQueueFamilyProperties, NULL);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &property_count, properties);
    array_set_length(properties, property_count);

    for (u32 i = 0; i < array_length(properties); i++) {
        VkQueueFamilyProperties property = properties[i];
        if (property.queueCount == 0) {
            continue;
        }

        if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT && indices.graphics == -1) {
            indices.graphics = i;
        }

        if (property.queueFlags & VK_QUEUE_COMPUTE_BIT
            && !(property.queueFlags & VK_QUEUE_GRAPHICS_BIT) && indices.compute == -1) {
            indices.compute = i;
        }

        if (property.queueFlags & VK_QUEUE_TRANSFER_BIT
            && !(property.queueFlags & VK_QUEUE_GRAPHICS_BIT) && indices.transfer == -1) {
            indices.transfer = i;
        }

        VkBool32 present = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present);
        if (present && indices.present == -1) {
            indices.present = i;
        }
    }

    if (indices.compute == -1) {
        indices.compute = indices.graphics;
    }

    if (indices.transfer == -1) {
        indices.transfer = indices.graphics;
    }

    if (indices.present == -1) {
        indices.present = indices.graphics;
    }

    array_destroy(properties);
    return indices;
}

static i32 vulkan_device_score(VkPhysicalDevice device, const char** required) {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);

    if (!vulkan_device_supports_extensions(device, required)) {
        return 0;
    }

    i32 score = 1;

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    score += properties.limits.maxImageDimension2D;

    return score;
}

b8 vulkan_device_create(VkInstance instance, VkSurfaceKHR surface, VulkanDevice* device) {
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (device_count == 0) {
        FATAL("No GPUs supporting Vulkan found!");
        return false;
    }

    VkPhysicalDevice* devices = array_sized(device_count, VkPhysicalDevice, NULL);
    vkEnumeratePhysicalDevices(instance, &device_count, devices);
    array_set_length(devices, device_count);

    const char** required_extensions = array(const char*, NULL);
    array_append(required_extensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if defined(VARA_PLATFORM_APPLE)
    array_append(required_extensions, "VK_KHR_portability_subset");
#endif

    i32 last_score = 0;
    VkPhysicalDevice selected = VK_NULL_HANDLE;
    DEBUG("Found %u Vulkan device(s):", device_count);
    for (u32 i = 0; i < array_length(devices); i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        DEBUG("\t- %s", properties.deviceName);

        const i32 score = vulkan_device_score(devices[i], required_extensions);
        if (score > last_score) {
            last_score = score;
            selected = devices[i];
        }
    }
    array_destroy(devices);

    if (selected == VK_NULL_HANDLE) {
        FATAL("Failed to find a supported GPU!");
        array_destroy(required_extensions);
        return false;
    }

    device->physical_device = selected;

    u32 format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical_device, surface, &format_count, NULL);
    device->surface_info.formats = array_sized(format_count, VkSurfaceFormatKHR, NULL);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device->physical_device, surface, &format_count, device->surface_info.formats
    );
    array_set_length(device->surface_info.formats, format_count);

    u32 present_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->physical_device, surface, &present_count, NULL
    );
    device->surface_info.present_modes = array_sized(present_count, VkPresentModeKHR, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->physical_device, surface, &present_count, device->surface_info.present_modes
    );
    array_set_length(device->surface_info.present_modes, present_count);

    vkGetPhysicalDeviceProperties(selected, &device->properties);
    vkGetPhysicalDeviceFeatures(selected, &device->features);
    vkGetPhysicalDeviceMemoryProperties(selected, &device->memory);

    // TODO: check for unique indices, this works as is on macOS since they all share one queue.
    QueueFamilyIndices indices =
        vulkan_device_find_queue_families(device->physical_device, surface);
    device->graphics_queue_index = indices.graphics;
    device->present_queue_index = indices.present;
    device->transfer_queue_index = indices.transfer;
    device->compute_queue_index = indices.compute;

    f32 priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = indices.graphics,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    VkPhysicalDeviceDriverProperties driver = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES
    };
    VkPhysicalDeviceProperties2 properties2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = &driver,
    };

    vkGetPhysicalDeviceProperties2(selected, &properties2);
    DEBUG("Using GPU named('%s'):", device->properties.deviceName);
    DEBUG("\tDriver: %s (%s)", driver.driverName, driver.driverInfo);

    const VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = array_length(required_extensions),
        .ppEnabledExtensionNames = required_extensions,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_info,
    };
    vkCreateDevice(device->physical_device, &info, NULL, &device->logical_device);
    array_destroy(required_extensions);
    volkLoadDevice(device->logical_device);

    vkGetDeviceQueue(
        device->logical_device, device->graphics_queue_index, 0, &device->graphics_queue
    );
    vkGetDeviceQueue(
        device->logical_device, device->present_queue_index, 0, &device->present_queue
    );
    vkGetDeviceQueue(
        device->logical_device, device->transfer_queue_index, 0, &device->transfer_queue
    );
    vkGetDeviceQueue(
        device->logical_device, device->compute_queue_index, 0, &device->compute_queue
    );

    const u32 major = VK_VERSION_MAJOR(device->properties.apiVersion);
    const u32 minor = VK_VERSION_MINOR(device->properties.apiVersion);
    const u32 patch = VK_VERSION_PATCH(device->properties.apiVersion);
    DEBUG("Using Vulkan API %u.%u.%u", major, minor, patch);

    return true;
}

void vulkan_device_destroy(VulkanDevice* device) {
    if (!device || !device->logical_device) {
        return;
    }

    vkDeviceWaitIdle(device->logical_device);
    // Get allocator callbacks somehow?
    vkDestroyDevice(device->logical_device, NULL);
    array_destroy(device->surface_info.formats);
    array_destroy(device->surface_info.present_modes);
}
