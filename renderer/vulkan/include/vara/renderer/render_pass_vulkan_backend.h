#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/render_pass.h>

#include "vara/renderer/vulkan_common.h"

typedef struct VulkanRenderPassState {
    VkRenderPass render_pass;
    b8 has_depth;
    b8 active;
} VulkanRenderPassState;

struct RenderTarget;

b8 render_pass_vulkan_create(RenderPass* pass, const RenderPassConfig* config);
void render_pass_vulkan_destroy(RenderPass* pass);
void render_pass_vulkan_begin(RenderPass* pass, struct RenderTarget* target);
void render_pass_vulkan_end(RenderPass* pass);