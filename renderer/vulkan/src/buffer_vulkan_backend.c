#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/buffer_vulkan_backend.h"
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

static VkBufferUsageFlags type_to_vk_usage(BufferType type) {
    switch (type) {
        case BUFFER_TYPE_VERTEX:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case BUFFER_TYPE_INDEX:
            return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case BUFFER_TYPE_UNIFORM:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        default:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
}

b8 buffer_vulkan_create(Buffer* buffer, const BufferConfig* config) {
    if (!buffer || !config) {
        return false;
    }

    VulkanBufferState* state = vara_allocate(sizeof(VulkanBufferState));
    vara_zero_memory(state, sizeof(VulkanBufferState));
    if (!state) {
        return false;
    }

    buffer->backend_data = state;
    state->size = config->size;

    VkBufferUsageFlags usage = type_to_vk_usage(config->type);
    usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocation_info = {0};
    if (config->usage == BUFFER_USAGE_STATIC) {
        allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    } else {
        allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    }

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = state->size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VulkanRendererState* renderer = buffer->backend->backend_data;
    VkResult result = vmaCreateBuffer(
        renderer->vma_allocator,
        &buffer_info,
        &allocation_info,
        &state->buffer,
        &state->allocation,
        NULL
    );
    if (result != VK_SUCCESS) {
        vara_free(state, sizeof(VulkanBufferState));
        return false;
    }

    if (config->data && config->size > 0) {
        if (config->usage == BUFFER_USAGE_STATIC) {
            buffer_vulkan_set_data(buffer, config->data, config->size, 0);
        } else {
            buffer_vulkan_set_data(buffer, config->data, config->size, 0);
        }
    }

    return true;
}

void buffer_vulkan_destroy(Buffer* buffer) {
    if (!buffer || !buffer->backend_data) {
        return;
    }

    VulkanBufferState* state = buffer->backend_data;
    VulkanRendererState* renderer = buffer->backend->backend_data;

    vkDeviceWaitIdle(renderer->device.logical_device);

    vmaDestroyBuffer(renderer->vma_allocator, state->buffer, state->allocation);

    vara_free(state, sizeof(VulkanBufferState));
    buffer->backend_data = NULL;
}

void buffer_vulkan_set_data(Buffer* buffer, const void* data, size_t size, size_t offset) {
    if (!buffer || !buffer->backend_data) {
        return;
    }

    VulkanBufferState* state = buffer->backend_data;
    VulkanRendererState* renderer = buffer->backend->backend_data;

    void* mapped = NULL;
    vmaMapMemory(renderer->vma_allocator, state->allocation, &mapped);
    vara_copy_memory((u8*)mapped + offset, data, size);
    vmaUnmapMemory(renderer->vma_allocator, state->allocation);
}
