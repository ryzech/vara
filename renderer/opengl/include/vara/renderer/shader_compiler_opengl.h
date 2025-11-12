#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/shader.h>

u32 shader_compiler_opengl_compile(const ShaderConfig* config);

void shader_compiler_opengl_delete(u32 program);
