#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/render_pass.h>

b8 render_pass_vulkan_create(RenderPass* pass, const RenderPassConfig* config);
void render_pass_vulkan_destroy(RenderPass* pass);
void render_pass_vulkan_begin(RenderPass* pass);
void render_pass_vulkan_end(RenderPass* pass);