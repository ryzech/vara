#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/render_pass_vulkan_backend.h"
#include "vara/renderer/render_target_vulkan_backend.h"
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/swapchain_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

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

    VkAttachmentDescription attachments[8];
    VkAttachmentReference color_refs[8];
    u32 attachment_index = 0;

    for (u32 i = 0; i < pass->color_attachment_count; i++) {
        attachments[attachment_index] = (VkAttachmentDescription){
            .format = VK_FORMAT_UNDEFINED,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = get_vulkan_load_op(pass->color_attachments[i].load),
            .storeOp = get_vulkan_store_op(pass->color_attachments[i].store),
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        color_refs[i] = (VkAttachmentReference){
            .attachment = attachment_index,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        attachment_index++;
    }

    VkAttachmentReference depth_ref = {};
    if (pass->depth_stencil_attachment) {
        attachments[attachment_index] = (VkAttachmentDescription){
            .format = VK_FORMAT_UNDEFINED,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = get_vulkan_load_op(pass->depth_stencil_attachment->load),
            .storeOp = get_vulkan_store_op(pass->depth_stencil_attachment->store),
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        depth_ref = (VkAttachmentReference){
            .attachment = attachment_index,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        attachment_index++;
    }

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = pass->color_attachment_count,
        .pColorAttachments = color_refs,
        .pDepthStencilAttachment = state->has_depth ? &depth_ref : NULL,
    };

    VkRenderPassCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = attachment_index,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
    };

    VK_CHECK(vkCreateRenderPass(
        renderer->device.logical_device, &info, renderer->allocator, &state->render_pass
    ));

    state->active = false;
    return true;
}

void render_pass_vulkan_destroy(RenderPass* pass) {
    if (!pass || !pass->backend_data) {
        return;
    }

    VulkanRenderPassState* state = pass->backend_data;
    VulkanRendererState* renderer = pass->backend->backend_data;

    vkDeviceWaitIdle(renderer->device.logical_device);

    vkDestroyRenderPass(renderer->device.logical_device, state->render_pass, renderer->allocator);
    vara_free(state, sizeof(VulkanRenderPassState));
    pass->backend_data = NULL;
}

void render_pass_vulkan_begin(RenderPass* pass, RenderTarget* target) {
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

    VkClearValue clear_value[8];
    u32 clear_count = 0;

    for (u32 i = 0; i < pass->color_attachment_count; i++) {
        Vector4 c = pass->color_attachments[i].clear;
        clear_value[clear_count++].color = (VkClearColorValue){{
            c.x,
            c.y,
            c.z,
            c.w,
        }};
    }

    if (pass->depth_stencil_attachment) {
        clear_value[clear_count++].depthStencil = (VkClearDepthStencilValue){
            1.0f,
            0,
        };
    }

    VulkanRenderTargetState* target_state = target->backend_data;
    VkRenderPassBeginInfo render_pass_begin = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = state->render_pass,
        .framebuffer = target_state->framebuffer,
        .renderArea =
            {
                .offset = {0, 0},
                .extent = {target->width, target->height},
            },
        .clearValueCount = clear_count,
        .pClearValues = clear_value,
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
