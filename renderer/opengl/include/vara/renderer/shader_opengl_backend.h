#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/shader.h>

b8 shader_opengl_create(Shader* shader, const ShaderConfig* config);
void shader_opengl_destroy(Shader* shader);
void shader_opengl_bind(Shader* shader);
void shader_opengl_unbind(Shader* shader);
void shader_opengl_set_mat4(Shader* shader, const char* name, Matrix4 matrix);
void shader_opengl_set_int_array(Shader* shader, const char* name, const i32* array, u32 count);
void shader_opengl_dispatch(Shader* shader, i16 x, i16 y, i16 z);