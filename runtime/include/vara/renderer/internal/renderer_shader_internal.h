#pragma once

#include "vara/renderer/shader.h"

typedef struct ShaderBackendVT ShaderBackendVT;

struct ShaderBackendVT {
    b8 (*create)(Shader* shader, const ShaderConfig* config);
    void (*destroy)(Shader* shader);
    void (*bind)(Shader* shader);
    void (*unbind)(Shader* shader);
    // TODO: abstract uniforms away
    void (*set_mat4)(Shader* shader, const char* name, Matrix4 matrix);
    void (*set_int_array)(Shader* shader, const char* name, const i32* array, u32 count);
    void (*dispatch)(Shader* shader, i16 x, i16 y, i16 z);
};