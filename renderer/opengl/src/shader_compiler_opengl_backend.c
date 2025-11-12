#include "vara/renderer/shader_compiler_opengl.h"

#include <glad/gl.h>

u32 shader_compiler_opengl_compile(const ShaderConfig* config) {
    u32 program = glCreateProgram();

    return program;
}

void shader_compiler_opengl_delete(u32 program) {
    if (program) {
        glDeleteProgram(program);
    }
}
