#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/render_pipeline.h>

#include "vara/renderer/vulkan_common.h"

typedef struct VulkanPipelineState {
    VkPipeline pipeline;
    VkPipelineLayout layout;
} VulkanPipelineState;

b8 render_pipeline_vulkan_create(RenderPipeline* pipeline, const RenderPipelineConfig* config);
void render_pipeline_vulkan_destroy(RenderPipeline* pipeline);
void render_pipeline_vulkan_bind(RenderPipeline* pipeline);