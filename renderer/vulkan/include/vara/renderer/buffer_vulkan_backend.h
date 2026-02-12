#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/buffer.h>

#include "vara/renderer/vulkan_common.h"

typedef struct VulkanBufferState {
    VkBuffer buffer;
    VmaAllocation allocation;
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags memory_properties;
} VulkanBufferState;

b8 buffer_vulkan_create(Buffer* buffer, const BufferConfig* config);
void buffer_vulkan_destroy(Buffer* buffer);
void buffer_vulkan_set_data(Buffer* buffer, const void* data, size_t size, size_t offset);