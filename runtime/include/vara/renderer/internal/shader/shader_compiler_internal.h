#pragma once

#include <vara/core/defines.h>

#include "vara/renderer/shader.h"

typedef struct CompiledShaderStage CompiledShaderStage;
typedef struct CompiledShader CompiledShader;

struct RendererBackend;

struct CompiledShaderStage {
    ShaderStage stage;
    void* bytecode;
    u32 bytecode_size;
};

struct CompiledShader {
    const char* name;
    CompiledShaderStage* stages;
    u16 stage_count;
};

CompiledShader* shader_compiler_compile(
    const ShaderConfig* config, struct RendererBackend* backend
);
void shader_compiler_release(CompiledShader* compiled);