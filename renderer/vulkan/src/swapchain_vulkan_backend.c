#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/swapchain_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

static VkSurfaceFormatKHR choose_surface_format(VkSurfaceFormatKHR* formats) {
    for (u32 i = 0; i < array_length(formats); i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM
            && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }

    return formats[0];
}

static VkPresentModeKHR choose_present_mode(VkPresentModeKHR* modes, b8 vsync) {
    if (!vsync) {
        for (u32 i = 0; i < array_length(modes); i++) {
            if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                return modes[i];
            }
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

b8 swapchain_vulkan_create(Swapchain* swapchain, const SwapchainConfig* config) {
    if (!swapchain || !config) {
        return false;
    }

    DEBUG("Creating Swapchain for VaraWindow named('%s')", config->window->name);
    VulkanSwapchainState* state = vara_allocate(sizeof(VulkanSwapchainState));
    vara_zero_memory(state, sizeof(VulkanSwapchainState));
    if (!state) {
        return false;
    }

    VulkanRendererState* renderer = swapchain->backend->backend_data;
    swapchain->backend_data = state;

    VkExtent2D extent = {
        config->window->framebuffer_width,
        config->window->framebuffer_height,
    };

    state->image_format = choose_surface_format(renderer->device.surface_info.formats);
    state->present_mode =
        choose_present_mode(renderer->device.surface_info.present_modes, config->vsync);
    state->extent = extent;

    u32 image_count = renderer->device.surface_info.capabilities.minImageCount + 1;
    if (renderer->device.surface_info.capabilities.maxImageCount > 0
        && image_count > renderer->device.surface_info.capabilities.maxImageCount) {
        image_count = renderer->device.surface_info.capabilities.maxImageCount;
    }
    state->image_count = image_count;

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = renderer->surface,
        .minImageCount = image_count,
        .imageFormat = state->image_format.format,
        .imageColorSpace = state->image_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .preTransform = renderer->device.surface_info.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = state->present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    if (renderer->device.graphics_queue_index != renderer->device.present_queue_index) {
        const u32 indices[] = {
            renderer->device.graphics_queue_index,
            renderer->device.present_queue_index,
        };
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = indices;
    } else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VkResult result = vkCreateSwapchainKHR(
        renderer->device.logical_device, &swapchain_info, renderer->allocator, &state->swapchain
    );
    if (result != VK_SUCCESS) {
        FATAL("Failed to create VkSwapchain! Code: %u", result);
        return false;
    }

    result = vkGetSwapchainImagesKHR(
        renderer->device.logical_device, state->swapchain, &state->image_count, NULL
    );
    if (result != VK_SUCCESS) {
        FATAL("Failed to get Swapchain image count! Code: %u", result);
        return false;
    }

    state->images = array_sized(state->image_count, VkImage, NULL);
    result = vkGetSwapchainImagesKHR(
        renderer->device.logical_device, state->swapchain, &state->image_count, state->images
    );
    if (result != VK_SUCCESS) {
        FATAL("Failed to get Swapchain images! Code: %u", result);
        return false;
    }
    array_set_length(state->images, state->image_count);

    state->views = array_sized(state->image_count, VkImageView, NULL);
    for (u32 i = 0; i < state->image_count; i++) {
        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = state->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = state->image_format.format,
            .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };

        VK_CHECK(vkCreateImageView(
            renderer->device.logical_device, &view_info, renderer->allocator, &state->views[i]
        ));
    }

    return true;
}

void swapchain_vulkan_destroy(Swapchain* swapchain) {
    VulkanSwapchainState* state = swapchain->backend_data;
    VulkanRendererState* renderer = swapchain->backend->backend_data;

    vkDestroySwapchainKHR(renderer->device.logical_device, state->swapchain, renderer->allocator);
    array_destroy(state->views);
    array_destroy(state->images);
    vara_free(state, sizeof(VulkanSwapchainState));
}

void swapchain_vulkan_present(Swapchain* swapchain) {
    if (!swapchain || !swapchain->backend_data) {
        return;
    }

    VulkanSwapchainState* state = swapchain->backend_data;
    VulkanRendererState* renderer = swapchain->backend->backend_data;
    VulkanFrame frame = renderer->frames[renderer->current_frame];

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &frame.render_finished,
        .swapchainCount = 1,
        .pSwapchains = &state->swapchain,
        .pImageIndices = &renderer->image_index,
    };

    vkQueuePresentKHR(renderer->device.present_queue, &present_info);
}
