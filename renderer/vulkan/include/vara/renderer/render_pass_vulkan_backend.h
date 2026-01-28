#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/render_pass.h>

#include "volk/volk.h"

typedef struct VulkanRenderPassState {
    VkRenderPass render_pass;
    VkFramebuffer* framebuffers;
    u32 framebuffer_count;
    u32 current_framebuffer_index;
    b8 active;
} VulkanRenderPassState;

b8 render_pass_vulkan_create(RenderPass* pass, const RenderPassConfig* config);
void render_pass_vulkan_destroy(RenderPass* pass);
void render_pass_vulkan_begin(RenderPass* pass);
void render_pass_vulkan_end(RenderPass* pass);