#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/shader.h>

#include "volk/volk.h"

typedef struct VulkanShaderStage {
    ShaderStage stage;
    VkShaderModule module;
    const char* entrypoint;
} VulkanShaderStage;

typedef struct VulkanShaderState {
    u32 stage_count;
    VulkanShaderStage* stages;
} VulkanShaderState;

b8 shader_vulkan_create(Shader* shader, const ShaderConfig* config);
void shader_vulkan_destroy(Shader* shader);
void shader_vulkan_bind(Shader* shader);
void shader_vulkan_unbind(Shader* shader);
void shader_vulkan_set_mat4(Shader* shader, const char* name, Matrix4 matrix);
void shader_vulkan_set_int_array(Shader* shader, const char* name, const i32* array, u32 count);
void shader_vulkan_dispatch(Shader* shader, i16 x, i16 y, i16 z);