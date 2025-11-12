#pragma once

#include <vara/core/defines.h>

#include "vara/renderer/renderer.h"

typedef enum ShaderStage {
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_FRAGMENT,
    SHADER_STAGE_COMPUTE
} ShaderStage;

// TODO: this needs re-wrote so one file can have frag and vert
typedef struct ShaderSource {
    ShaderStage stage;
    const char* source;
} ShaderSource;

typedef struct ShaderConfig {
    const char* name;
    ShaderSource* stages;
    u16 stage_count;
} ShaderConfig;

typedef struct ShaderVT {
    b8 (*shader_create)(struct Shader* shader, ShaderConfig* config);
    void (*shader_destroy)(struct Shader* shader);
    void (*shader_bind)(struct Shader* shader);
    void (*shader_unbind)(struct Shader* shader);
} ShaderVT;

typedef struct Shader {
    const char* name;
    ShaderVT vt;
    void* backend_data;
} Shader;

Shader* shader_create(RendererInstance* instance, ShaderConfig* config);

void shader_destroy(Shader* shader);

void shader_bind(Shader* shader);

void shader_unbind(Shader* shader);
