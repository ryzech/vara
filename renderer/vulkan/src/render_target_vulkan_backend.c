#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/render_target_vulkan_backend.h"
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/texture_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

b8 render_target_vulkan_create(RenderTarget* target, const RenderTargetConfig* config) {
    if (!target || !config) {
        return false;
    }

    VulkanRenderTargetState* state = vara_allocate(sizeof(VulkanRenderTargetState));
    vara_zero_memory(state, sizeof(VulkanRenderTargetState));
    if (!state) {
        return false;
    }

    target->backend_data = state;

    VkImageView attachments[8];
    u32 attachment_count = 0;

    for (u32 i = 0; i < target->attachment_count; i++) {
        Texture* texture = target->attachments[i].texture;
        VulkanTextureState* texture_state = texture->backend_data;
        attachments[attachment_count++] = texture_state->view;
    }

    state->framebuffer = VK_NULL_HANDLE;
    state->attachment_count = attachment_count;
    state->attachments = vara_allocate(sizeof(VkImageView) * attachment_count);
    vara_copy_memory(state->attachments, attachments, sizeof(VkImageView) * attachment_count);

    return true;
}

void render_target_vulkan_destroy(RenderTarget* target) {
}

void render_target_vulkan_resize(RenderTarget* target, u32 width, u32 height) {
}
