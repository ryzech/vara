#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/render_pipeline_vulkan_backend.h"
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/shader_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

b8 render_pipeline_vulkan_create(RenderPipeline* pipeline, const RenderPipelineConfig* config) {
    if (!pipeline || !config) {
        return false;
    }

    DEBUG("Creating RenderPipeline named('%s')", config->name);
    VulkanPipelineState* state = vara_allocate(sizeof(VulkanPipelineState));
    vara_zero_memory(state, sizeof(VulkanPipelineState));
    if (!state) {
        return false;
    }

    VulkanRendererState* renderer = pipeline->backend->backend_data;
    pipeline->backend_data = state;

    // VkGraphicsPipelineCreateInfo pipeline_info = {
    //     .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    // };
    //
    // VkResult result = vkCreateGraphicsPipelines(
    //     renderer->device.logical_device,
    //     VK_NULL_HANDLE,
    //     1,
    //     &pipeline_info,
    //     renderer->allocator,
    //     &state->pipeline
    // );
    // if (result != VK_SUCCESS) {
    //     ERROR("Failed to create RenderPipeline named('%s')", pipeline->name);
    //     return false;
    // }

    return true;
}

void render_pipeline_vulkan_destroy(RenderPipeline* pipeline) {
    if (!pipeline || !pipeline->backend_data) {
        return;
    }

    VulkanPipelineState* state = pipeline->backend_data;
    VulkanRendererState* renderer = pipeline->backend->backend_data;

    vkDestroyPipeline(renderer->device.logical_device, state->pipeline, renderer->allocator);
    vkDestroyPipelineLayout(renderer->device.logical_device, state->layout, renderer->allocator);

    vara_free(state, sizeof(VulkanPipelineState));
    pipeline->backend_data = NULL;
}

void render_pipeline_vulkan_bind(RenderPipeline* pipeline) {
    if (!pipeline || !pipeline->backend_data) {
        return;
    }

    VulkanPipelineState* state = pipeline->backend_data;
    VulkanRendererState* renderer = pipeline->backend->backend_data;
}
