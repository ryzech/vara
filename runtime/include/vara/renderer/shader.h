#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

// Forward declarations, in case they need access to each other.
typedef enum ShaderStage ShaderStage;
typedef struct ShaderSource ShaderSource;
typedef struct ShaderConfig ShaderConfig;
typedef struct ShaderVT ShaderVT;
typedef struct Shader Shader;

enum ShaderStage {
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_FRAGMENT,
    SHADER_STAGE_COMPUTE
};

// TODO: this needs re-wrote so one file can have frag and vert
struct ShaderSource {
    ShaderStage stage;
    const char* source;
};

struct ShaderConfig {
    const char* name;
    ShaderSource* stages;
    u16 stage_count;
};

struct ShaderVT {
    b8 (*shader_create)(Shader* shader, const ShaderConfig* config);
    void (*shader_destroy)(Shader* shader);
    void (*shader_bind)(Shader* shader);
    void (*shader_unbind)(Shader* shader);
    // TODO: abstract uniforms away
    void (*shader_set_mat4)(
        Shader* shader, const char* name, const Matrix4* matrix
    );
    void (*shader_dispatch)(Shader* shader, i16 x, i16 y, i16 z);
};

struct Shader {
    const char* name;
    ShaderVT vt;
    void* backend_data;
};

Shader* shader_create(const ShaderConfig* config);
void shader_destroy(Shader* shader);

void shader_bind(Shader* shader);
void shader_unbind(Shader* shader);
void shader_set_mat4(Shader* shader, const char* name, const Matrix4* matrix);
void shader_dispatch(Shader* shader, i16 x, i16 y, i16 z);
