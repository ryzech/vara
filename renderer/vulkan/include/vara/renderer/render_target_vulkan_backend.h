#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/render_target.h>

#include "vara/renderer/vulkan_common.h"

typedef struct VulkanRenderTargetState {
    VkFramebuffer framebuffer;
} VulkanRenderTargetState;

b8 render_target_vulkan_create(RenderTarget* target, const RenderTargetConfig* config);
void render_target_vulkan_destroy(RenderTarget* target);
void render_target_vulkan_resize(RenderTarget* target, u32 width, u32 height);