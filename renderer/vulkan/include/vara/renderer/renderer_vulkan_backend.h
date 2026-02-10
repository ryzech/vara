#pragma once

#include <vara/core/platform/platform_window.h>

#include "swapchain_vulkan_backend.h"
#include "vara/renderer/vulkan_device.h"
#include "vma/vk_mem_alloc.h"
#include "volk/volk.h"

typedef struct VulkanRendererState VulkanRendererState;

struct VulkanRendererState {
    VaraWindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VulkanDevice device;
    VkAllocationCallbacks* allocator;
    VmaAllocator vma_allocator;
    Swapchain* swapchain;
    VkDescriptorPool descriptor_pool;
};