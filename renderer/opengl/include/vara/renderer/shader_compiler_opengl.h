#pragma once

#include <glad/gl.h>
#include <vara/core/defines.h>
#include <vara/renderer/shader.h>

GLuint shader_compiler_opengl_compile(const ShaderConfig* config);

void shader_compiler_opengl_delete(GLuint program);
