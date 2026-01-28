#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/render_pass_vulkan_backend.h"
#include "vara/renderer/render_pipeline_vulkan_backend.h"
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/shader_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

static VkShaderStageFlagBits shader_stage_to_vk(ShaderStage stage) {
    switch (stage) {
        case SHADER_STAGE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case SHADER_STAGE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SHADER_STAGE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_VERTEX_BIT;
    }
}

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

    VulkanShaderState* shader = pipeline->shader->backend_data;
    VulkanRenderPassState* pass = pipeline->pass->backend_data;

    VkPipelineShaderStageCreateInfo stage_info[shader->stage_count];
    for (u32 i = 0; i < shader->stage_count; i++) {
        VulkanShaderStage* stage = &shader->stages[i];
        stage_info[i] = (VkPipelineShaderStageCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = shader_stage_to_vk(stage->stage),
            .module = stage->module,
            .pName = stage->entrypoint,
            .pSpecializationInfo = NULL,
        };
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo viewport_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = NULL,
        .scissorCount = 1,
        .pScissors = NULL,
    };

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
    };
    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 3,
        .pDynamicStates = dynamic_states,
    };

    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };
    VK_CHECK(vkCreatePipelineLayout(
        renderer->device.logical_device, &layout_info, renderer->allocator, &state->layout
    ));

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = shader->stage_count,
        .pStages = stage_info,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pViewportState = &viewport_info,
        .pDynamicState = &dynamic_state_info,
        .layout = state->layout,
        .renderPass = pass->render_pass,
        .subpass = 0,
    };
    VkResult result = vkCreateGraphicsPipelines(
        renderer->device.logical_device,
        VK_NULL_HANDLE,
        1,
        &pipeline_info,
        renderer->allocator,
        &state->pipeline
    );
    if (result != VK_SUCCESS) {
        ERROR("Failed to create RenderPipeline named('%s')", pipeline->name);
        return false;
    }

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
    VulkanSwapchainState* swapchain = renderer->swapchain->backend_data;
    VulkanFrame* frame = &swapchain->frames[swapchain->current_frame];

    vkCmdBindPipeline(frame->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state->pipeline);
}
