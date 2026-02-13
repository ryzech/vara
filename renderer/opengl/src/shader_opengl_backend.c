#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/renderer/internal/renderer_internal.h>
#include <vara/renderer/internal/shader/shader_compiler_internal.h>

#include "vara/renderer/shader_opengl_backend.h"

b8 shader_opengl_create(Shader* shader, const ShaderConfig* config) {
    DEBUG("Creating shader program named('%s')", config->name);
    OpenGLShaderState* state = vara_allocate(sizeof(OpenGLShaderState));
    vara_zero_memory(state, sizeof(OpenGLShaderState));
    if (!state) {
        return false;
    }

    CompiledShader* compiled = shader_compiler_compile(config, shader->backend);
    if (!compiled) {
        ERROR("Failed to compile shader named('%s')", config->name);
        return false;
    }

    vara_copy_memory(&state->reflection, &compiled->reflection, sizeof(ReflectedShader));

    const GLuint program = glCreateProgram();
    for (u32 i = 0; i < compiled->stage_count; i++) {
        CompiledShaderStage* stage = &compiled->stages[i];

        GLenum gl_stage;
        switch (stage->stage) {
            case SHADER_STAGE_VERTEX:
                gl_stage = GL_VERTEX_SHADER;
                break;
            case SHADER_STAGE_FRAGMENT:
                gl_stage = GL_FRAGMENT_SHADER;
                break;
            case SHADER_STAGE_COMPUTE:
                gl_stage = GL_COMPUTE_SHADER;
                break;
            default:
                gl_stage = GL_VERTEX_SHADER;
                break;
        }

        const GLuint gl_shader = glCreateShader(gl_stage);
        const char* source = stage->bytecode;
        glShaderSource(gl_shader, 1, &source, NULL);
        glCompileShader(gl_shader);

        GLint success;
        glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char info[1024];
            glGetShaderInfoLog(gl_shader, sizeof(info), NULL, info);
            ERROR("OpenGL shader compile error:\n%s", info);

            glDeleteShader(gl_shader);
            glDeleteProgram(program);
            shader_compiler_release(compiled);
            vara_free(state, sizeof(OpenGLShaderState));
            return false;
        }

        glAttachShader(program, gl_shader);
        glDeleteShader(gl_shader);
    }

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[1024];
        glGetProgramInfoLog(program, sizeof(info), NULL, info);
        ERROR("OpenGL program link error:\n%s", info);

        glDeleteProgram(program);
        shader_compiler_release(compiled);
        vara_free(state, sizeof(OpenGLShaderState));
        return false;
    }

    state->shader_program = program;
    shader_compiler_release(compiled);

    shader->backend_data = state;
    return true;
}

void shader_opengl_destroy(Shader* shader) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* state = shader->backend_data;
    if (state->shader_program) {
        glDeleteProgram(state->shader_program);
    }

    vara_free(state, sizeof(OpenGLShaderState));
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