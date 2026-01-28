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

    VkVertexInputBindingDescription binding_description = {
        .binding = 0,
        .stride = 0,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription attribute_descriptions[32];
    u32 offset = 0;
    for (u32 i = 0; i < shader->reflection.vertex_attribute_count; i++) {
        ReflectedVertexAttribute* attribute = &shader->reflection.vertex_attributes[i];
        attribute_descriptions[i] = (VkVertexInputAttributeDescription){
            .location = attribute->location,
            .binding = 0,
            .format = vertex_attribute_type_to_vk(attribute->type),
            .offset = offset,
        };
        offset += vertex_attribute_type_size(attribute->type);
    }
    binding_description.stride = offset;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_description,
        .vertexAttributeDescriptionCount = shader->reflection.vertex_attribute_count,
        .pVertexAttributeDescriptions = attribute_descriptions,
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

    VkPipelineRasterizationStateCreateInfo rasterization_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthClampEnable = VK_FALSE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisample_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
    };

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
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
        .pRasterizationState = &rasterization_info,
        .pMultisampleState = &multisample_info,
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
