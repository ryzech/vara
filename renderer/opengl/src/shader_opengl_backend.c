#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/shader_compiler_opengl.h"
#include "vara/renderer/shader_opengl_backend.h"

typedef struct OpenGLShaderState {
    GLuint shader_program;
} OpenGLShaderState;

b8 shader_opengl_create(Shader* shader, const ShaderConfig* config) {
    DEBUG("Creating shader program named('%s')", config->name);
    OpenGLShaderState* shader_state = vara_allocate(sizeof(OpenGLShaderState));
    vara_zero_memory(shader_state, sizeof(OpenGLShaderState));
    if (!shader_state) {
        return false;
    }

    shader_state->shader_program = shader_compiler_opengl_compile(config);
    if (shader_state->shader_program == 0) {
        vara_free(shader_state, sizeof(OpenGLShaderState));
        return false;
    }

    shader->backend_data = shader_state;
    return true;
}

void shader_opengl_destroy(Shader* shader) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* shader_state = shader->backend_data;
    if (shader_state->shader_program) {
        shader_compiler_opengl_delete(shader_state->shader_program);
        shader_state->shader_program = 0;
    }

    vara_free(shader_state, sizeof(OpenGLShaderState));
    shader->backend_data = NULL;
}

void shader_opengl_bind(Shader* shader) {
    if (!shader || !shader->backend_data) {
        return;
    }

    const OpenGLShaderState* shader_state = shader->backend_data;
    glUseProgram(shader_state->shader_program);
}

void shader_opengl_unbind(Shader* shader) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* shader_state = shader->backend_data;

    TRACE(
        "Unbinding shader named('%s') in program(%d)", shader->name, shader_state->shader_program
    );
    glUseProgram(0);
}

void shader_opengl_set_mat4(Shader* shader, const char* name, Matrix4 matrix) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* shader_state = shader->backend_data;

    GLint location = glGetUniformLocation(shader_state->shader_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.elements);
}

void shader_opengl_set_int_array(Shader* shader, const char* name, const i32* array, u32 count) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* shader_state = shader->backend_data;

    GLint location = glGetUniformLocation(shader_state->shader_program, name);
    glUniform1iv(location, (GLint)count, array);
}

void shader_opengl_dispatch(Shader* shader, i16 x, i16 y, i16 z) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* shader_state = shader->backend_data;

    TRACE(
        "Dispatching compute shader named('%s') with workgroup(%d, %d, %d) in "
        "program(%d)",
        shader->name,
        x,
        y,
        z,
        shader_state->shader_program
    );
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}