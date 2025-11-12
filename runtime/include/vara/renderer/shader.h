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

typedef struct ShaderInfo {
    const char* name;
    ShaderSource* stages;
    u16 stage_count;
} ShaderInfo;

typedef struct ShaderVT {
    b8 (*shadet_create)(void);
    void (*shader_destroy)(void);
    void (*shader_bind)(void);
    void (*shader_unbind)(void);
} ShaderVT;

typedef struct Shader {
    const char* name;
    ShaderVT vt;
} Shader;

Shader* shader_create(RendererInstance instance, char* shader);

void shader_destroy(Shader* shader);

void shader_bind(Shader* shader);

void shader_unbind(Shader* shader);
