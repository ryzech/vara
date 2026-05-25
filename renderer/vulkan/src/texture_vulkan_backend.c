#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/texture_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

b8 texture_vulkan_create(Texture* texture, const TextureConfig* config) {
    if (!texture || !config) {
        return false;
    }

    VulkanTextureState* state = vara_allocate(sizeof(VulkanTextureState));
    vara_zero_memory(state, sizeof(VulkanTextureState));
    if (!state) {
        return false;
    }

    texture->backend_data = state;
    state->extent.width = config->width;
    state->extent.height = config->height;
    state->samples = config->samples;
    state->format = format_to_vk(config->format);
    state->aspect = VK_IMAGE_ASPECT_COLOR_BIT;

    if (config->swapchain_image) {
        state->owned = false;
        return true;
    }

    state->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    state->owned = true;

    VulkanRendererState* renderer = texture->backend->backend_data;

    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = state->format,
        .extent = {state->extent.width, state->extent.height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = state->samples > 1 ? VK_SAMPLE_COUNT_4_BIT : VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = state->usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo allocation_info = {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };

    VK_CHECK(vmaCreateImage(
        renderer->vma_allocator,
        &image_info,
        &allocation_info,
        &state->image,
        &state->allocation,
        NULL
    ));

    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = state->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = state->format,
        .subresourceRange = {state->aspect, 0, 1, 0, 1},
    };

    VK_CHECK(vkCreateImageView(
        renderer->device.logical_device, &view_info, renderer->allocator, &state->view
    ));

    return true;
}

void texture_vulkan_destroy(Texture* texture) {
    if (!texture || !texture->backend_data) {
        return;
    }

    VulkanTextureState* state = texture->backend_data;
    VulkanRendererState* renderer = texture->backend->backend_data;

    if (state->view) {
        vkDestroyImageView(renderer->device.logical_device, state->view, renderer->allocator);
    }
    if (state->owned && state->image) {
        vmaDestroyImage(renderer->vma_allocator, state->image, state->allocation);
    }

    vara_free(state, sizeof(VulkanTextureState));
    texture->backend_data = NULL;
}

void texture_vulkan_bind(Texture* texture, u32 slot) {
}

void texture_vulkan_unbind(Texture* texture) {
}

void texture_vulkan_set_data(Texture* texture, void* data, size_t size) {
    if (!texture || !texture->backend_data) {
        return;
    }

    VulkanTextureState* state = texture->backend_data;
    VulkanRendererState* renderer = texture->backend->backend_data;

    // void* mapped = NULL;
    // vmaMapMemory(renderer->vma_allocator, state->allocation, &mapped);
    // vara_copy_memory(mapped, data, size);
    // vmaUnmapMemory(renderer->vma_allocator, state->allocation);
}

// FIX: return a Texture pointer instead.
b8 texture_vulkan_wrap_image(Texture* texture, VkImage image, VkFormat format, VkExtent2D extent) {
    if (!texture || !image) {
        return false;
    }

    VulkanTextureState* state = vara_allocate(sizeof(VulkanTextureState));
    vara_zero_memory(state, sizeof(VulkanTextureState));
    if (!state) {
        return false;
    }

    texture->backend_data = state;
    state->image = image;
    state->format = format;
    state->extent = extent;
    state->aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    state->samples = VK_SAMPLE_COUNT_1_BIT;
    state->owned = false;

    VulkanRendererState* renderer = texture->backend->backend_data;
    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = state->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = state->format,
        .subresourceRange = {state->aspect, 0, 1, 0, 1},
    };
    VK_CHECK(vkCreateImageView(
        renderer->device.logical_device, &view_info, renderer->allocator, &state->view
    ));

    return true;
}
