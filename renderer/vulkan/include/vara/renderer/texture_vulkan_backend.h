#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/texture.h>

#include "vara/renderer/vulkan_common.h"

typedef struct VulkanTextureState {
    VkImage image;
    VkImageView view;
    VmaAllocation allocation;

    VkFormat format;
    VkExtent2D extent;
    VkImageUsageFlags usage;
    VkImageAspectFlags aspect;
    u32 samples;

    b8 owned;
} VulkanTextureState;

b8 texture_vulkan_create(Texture* texture, const TextureConfig* config);
void texture_vulkan_destroy(Texture* texture);
void texture_vulkan_bind(Texture* texture, u32 slot);
void texture_vulkan_unbind(Texture* texture);
void texture_vulkan_set_data(Texture* texture, void* data, size_t size);

b8 texture_vulkan_wrap_image(Texture* texture, VkImage image, VkFormat format, VkExtent2D extent);