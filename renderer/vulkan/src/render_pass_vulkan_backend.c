#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/render_pass_vulkan_backend.h"
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/swapchain_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

typedef struct VulkanRenderPassState {
    VkRenderPass render_pass;
    VkFramebuffer* framebuffers;
    u32 framebuffer_count;
    u32 current_framebuffer_index;
    b8 active;
} VulkanRenderPassState;

static VkAttachmentLoadOp get_vulkan_load_op(AttachmentLoadOp load_op) {
    switch (load_op) {
        case ATTACHMENT_LOAD_OP_LOAD:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case ATTACHMENT_LOAD_OP_CLEAR:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case ATTACHMENT_LOAD_OP_DONT_CARE:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

static VkAttachmentStoreOp get_vulkan_store_op(AttachmentStoreOp store_op) {
    switch (store_op) {
        case ATTACHMENT_STORE_OP_STORE:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case ATTACHMENT_STORE_OP_DONT_CARE:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default:
            return VK_ATTACHMENT_STORE_OP_STORE;
    }
}

b8 render_pass_vulkan_create(RenderPass* pass, const RenderPassConfig* config) {
    DEBUG("Creating render pass named('%s')", config->name);

    VulkanRenderPassState* state = vara_allocate(sizeof(VulkanRenderPassState));
    vara_zero_memory(state, sizeof(VulkanRenderPassState));
    if (!state) {
        return false;
    }

    pass->backend_data = state;
    VulkanRendererState* renderer = pass->backend->backend_data;

    if (pass->target) {

    } else {
        VulkanSwapchainState* swapchain = renderer->swapchain->backend_data;

        VkAttachmentDescription color_attachment = {
            .format = swapchain->image_format.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = get_vulkan_load_op(config->color_attachments[0].load),
            .storeOp = get_vulkan_store_op(config->color_attachments[0].store),
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };
        VkAttachmentReference color_attachment_reference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpass = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_reference,
        };
        VkSubpassDependency subpass_dependency = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        };

        VkRenderPassCreateInfo render_pass_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &color_attachment,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &subpass_dependency,
        };

        vkCreateRenderPass(
            renderer->device.logical_device,
            &render_pass_info,
            renderer->allocator,
            &state->render_pass
        );

        state->framebuffer_count = swapchain->image_count;
        state->framebuffers = vara_allocate(sizeof(VkFramebuffer) * state->framebuffer_count);

        for (u32 i = 0; i < state->framebuffer_count; i++) {
            VkImageView attachments[] = {swapchain->views[i]};

            VkFramebufferCreateInfo fb_info = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = state->render_pass,
                .attachmentCount = 1,
                .pAttachments = attachments,
                .width = swapchain->extent.width,
                .height = swapchain->extent.height,
                .layers = 1,
            };

            VK_CHECK(vkCreateFramebuffer(
                renderer->device.logical_device,
                &fb_info,
                renderer->allocator,
                &state->framebuffers[i]
            ));
        }
    }

    state->active = false;

    return true;
}

void render_pass_vulkan_destroy(RenderPass* pass) {
    if (!pass || !pass->backend_data) {
        return;
    }

    VulkanRenderPassState* state = pass->backend_data;
    VulkanRendererState* renderer = pass->backend->backend_data;

    for (u32 i = 0; i < state->framebuffer_count; i++) {
        vkDestroyFramebuffer(
            renderer->device.logical_device, state->framebuffers[i], renderer->allocator
        );
    }
    vara_free(state->framebuffers, sizeof(VkFramebuffer) * state->framebuffer_count);

    vkDestroyRenderPass(renderer->device.logical_device, state->render_pass, renderer->allocator);
    vara_free(state, sizeof(VulkanRenderPassState));
    pass->backend_data = NULL;
}

void render_pass_vulkan_begin(RenderPass* pass) {
    if (!pass || !pass->backend_data) {
        return;
    }

    VulkanRenderPassState* state = pass->backend_data;

    if (state->active) {
        WARN("Render pass named('%s') is already active!", pass->name);
        return;
    }

    VulkanRendererState* renderer = pass->backend->backend_data;
    VulkanSwapchainState* swapchain = renderer->swapchain->backend_data;
    VulkanFrame* frame = &swapchain->frames[swapchain->current_frame];
    state->current_framebuffer_index = swapchain->image_index;

    VkClearValue clear_value;
    if (pass->color_attachment_count > 0) {
        const Vector4 clear = pass->color_attachments[0].clear;
        clear_value.color = (VkClearColorValue){{clear.x, clear.y, clear.z, clear.w}};
    } else {
        clear_value.color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    }

    VkExtent2D extent;
    if (pass->target) {
        extent = (VkExtent2D){pass->target->width, pass->target->height};
    } else {
        extent = swapchain->extent;
    }

    VkRenderPassBeginInfo render_pass_begin = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = state->render_pass,
        .framebuffer = state->framebuffers[swapchain->image_index],
        .renderArea =
            {
                .offset = {0, 0},
                .extent = extent,
            },
        .clearValueCount = 1,
        .pClearValues = &clear_value,
    };

    vkCmdBeginRenderPass(frame->command_buffer, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);
    state->active = true;
}

void render_pass_vulkan_end(RenderPass* pass) {
    if (!pass || !pass->backend_data) {
        return;
    }

    VulkanRenderPassState* state = pass->backend_data;

    if (!state->active) {
        WARN("Render pass named('%s') is not active!", pass->name);
        return;
    }

    VulkanRendererState* renderer = pass->backend->backend_data;
    VulkanSwapchainState* swapchain = renderer->swapchain->backend_data;
    VulkanFrame* frame = &swapchain->frames[swapchain->current_frame];

    vkCmdEndRenderPass(frame->command_buffer);
    state->active = false;
}
