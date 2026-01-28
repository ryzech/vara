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
