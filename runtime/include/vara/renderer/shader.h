#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

// Forward declarations, in case they need access to each other.
typedef enum ShaderStage ShaderStage;
typedef struct ShaderSource ShaderSource;
typedef struct ShaderConfig ShaderConfig;
typedef struct Shader Shader;

struct Renderer;
struct RendererBackend;

enum ShaderStage {
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_FRAGMENT,
    SHADER_STAGE_COMPUTE
};

// TODO: this needs re-wrote so one file can have frag and vert
struct ShaderSource {
    const char* source;
    ShaderStage stage;
};

struct ShaderConfig {
    const char* name;
    ShaderSource* stages;
    u16 stage_count;
};

struct Shader {
    const char* name;
    void* backend_data;
    struct RendererBackend* backend;
};

Shader* shader_create(struct Renderer* renderer, const ShaderConfig* config);
void shader_destroy(Shader* shader);

void shader_bind(Shader* shader);
void shader_unbind(Shader* shader);
void shader_set_mat4(Shader* shader, const char* name, Matrix4 matrix);
void shader_set_int_array(Shader* shader, const char* name, const i32* array, u32 count);
void shader_dispatch(Shader* shader, i16 x, i16 y, i16 z);

static const char* shader_stage_name(ShaderStage stage) {
    switch (stage) {
        case SHADER_STAGE_VERTEX:
            return "vertex_shader";
        case SHADER_STAGE_FRAGMENT:
            return "fragment_shader";
        case SHADER_STAGE_COMPUTE:
            return "compute_shader";
        default:
            return "unknown";
    }
}
