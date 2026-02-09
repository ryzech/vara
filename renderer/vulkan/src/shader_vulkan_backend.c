#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>
#include <vara/renderer/internal/shader/shader_compiler_internal.h>

#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/shader_vulkan_backend.h"
#include "vara/renderer/vulkan_utils.h"

b8 shader_vulkan_create(Shader* shader, const ShaderConfig* config) {
    DEBUG("Creating shader program named('%s')", config->name);
    VulkanShaderState* state = vara_allocate(sizeof(VulkanShaderState));
    vara_zero_memory(state, sizeof(VulkanShaderState));
    if (!state) {
        return false;
    }

    CompiledShader* compiled = shader_compiler_compile(config, shader->backend);
    if (!compiled) {
        ERROR("Failed to compile shader named('%s')", config->name);
        return false;
    }

    state->stage_count = compiled->stage_count;
    state->stages = vara_allocate(sizeof(VulkanShaderStage) * state->stage_count);
    vara_copy_memory(&state->reflection, &compiled->reflection, sizeof(ReflectedShader));

    VulkanRendererState* renderer = shader->backend->backend_data;
    for (u32 i = 0; i < compiled->stage_count; i++) {
        CompiledShaderStage* src = &compiled->stages[i];
        VulkanShaderStage* dst = &state->stages[i];

        dst->stage = src->stage;
        dst->entrypoint = "main";

        VkShaderModuleCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = src->bytecode_size,
            .pCode = (const u32*)src->bytecode,
        };

        VK_CHECK(vkCreateShaderModule(
            renderer->device.logical_device, &create_info, renderer->allocator, &dst->module
        ));
    }

    if (compiled->reflection.descriptor_count > 0) {
        VkDescriptorSetLayoutBinding bindings[4][32];
        u32 binding_counts[4] = {0};

        for (u32 i = 0; i < compiled->reflection.descriptor_count; i++) {
            ReflectedDescriptor* descriptor = &compiled->reflection.descriptors[i];
            u32 set = descriptor->set;

            if (set >= 4) {
                continue;
            }

            u32 index = binding_counts[set]++;

            VkDescriptorType type;
            switch (descriptor->type) {
                case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    break;
                case DESCRIPTOR_TYPE_STORAGE_BUFFER:
                    type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;
                case DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                case DESCRIPTOR_TYPE_STORAGE_IMAGE:
                    type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    break;
                default:
                    type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }

            VkShaderStageFlags stage_flags = 0;
            if (descriptor->stage_mask & SHADER_STAGE_VERTEX) {
                stage_flags |= VK_SHADER_STAGE_VERTEX_BIT;
            }
            if (descriptor->stage_mask & SHADER_STAGE_FRAGMENT) {
                stage_flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            if (descriptor->stage_mask & SHADER_STAGE_COMPUTE) {
                stage_flags |= VK_SHADER_STAGE_COMPUTE_BIT;
            }

            bindings[set][index] = (VkDescriptorSetLayoutBinding){
                .binding = descriptor->binding,
                .descriptorType = type,
                .descriptorCount = descriptor->count,
                .stageFlags = stage_flags,
                .pImmutableSamplers = NULL,
            };
        }

        state->descriptor_set_layout_count = 0;
        for (u32 i = 0; i < 4; i++) {
            if (binding_counts[i] == 0) {
                continue;
            }

            VkDescriptorSetLayoutCreateInfo layout_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = binding_counts[i],
                .pBindings = bindings[i],
            };
            VK_CHECK(vkCreateDescriptorSetLayout(
                renderer->device.logical_device,
                &layout_info,
                renderer->allocator,
                &state->descriptor_set_layouts[i]
            ));

            state->descriptor_set_layout_count++;
        }
    }

    shader_compiler_release(compiled);

    shader->backend_data = state;
    return true;
}

void shader_vulkan_destroy(Shader* shader) {
    if (!shader || !shader->backend_data) {
        return;
    }

    VulkanShaderState* state = shader->backend_data;
    VulkanRendererState* renderer = shader->backend->backend_data;

    for (u32 i = 0; i < state->descriptor_set_layout_count; i++) {
        vkDestroyDescriptorSetLayout(
            renderer->device.logical_device, state->descriptor_set_layouts[i], renderer->allocator
        );
    }

    for (u32 i = 0; i < state->stage_count; i++) {
        if (state->stages[i].module) {
            vkDestroyShaderModule(
                renderer->device.logical_device, state->stages[i].module, renderer->allocator
            );
        }
    }

    vara_free(state->stages, sizeof(VulkanShaderStage) * state->stage_count);
    vara_free(state, sizeof(VulkanShaderState));
    shader->backend_data = NULL;
}

void shader_vulkan_bind(Shader* shader) {
}

void shader_vulkan_unbind(Shader* shader) {
}

void shader_vulkan_set_mat4(Shader* shader, const char* name, Matrix4 matrix) {
}

void shader_vulkan_set_int_array(Shader* shader, const char* name, const i32* array, u32 count) {
}

void shader_vulkan_dispatch(Shader* shader, i16 x, i16 y, i16 z) {
}
