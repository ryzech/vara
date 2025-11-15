#include <glad/gl.h>

#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/renderer/renderer.h>
#include <vara/renderer/shader.h>

#include "vara/renderer/shader_compiler_opengl.h"

typedef struct OpenGLShaderState {
    u32 shader_program;
} OpenGLShaderState;

static b8 shader_opengl_create(Shader* shader, const ShaderConfig* config) {
    DEBUG("Creating shader program named('%s')", config->name);
    OpenGLShaderState* shader_state =
        platform_allocate(sizeof(OpenGLShaderState));
    if (!shader_state) {
        return false;
    }

    shader_state->shader_program = shader_compiler_opengl_compile(config);
    if (shader_state->shader_program == 0) {
        platform_free(shader_state);
        return false;
    }

    shader->backend_data = shader_state;
    return true;
}

static void shader_opengl_destroy(Shader* shader) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* shader_state = (OpenGLShaderState*)shader->backend_data;
    if (shader_state->shader_program) {
        shader_compiler_opengl_delete(shader_state->shader_program);
        shader_state->shader_program = 0;
    }

    platform_free(shader_state);
    shader->backend_data = NULL;
}

static void shader_opengl_bind(Shader* shader) {
    if (!shader || !shader->backend_data) {
        return;
    }

    OpenGLShaderState* shader_state = (OpenGLShaderState*)shader->backend_data;
    glUseProgram(shader_state->shader_program);
}

static void shader_opengl_unbind(Shader* shader) {
    glUseProgram(0);
}

Shader* shader_opengl_init(const ShaderConfig* config) {
    Shader* opengl_shader = platform_allocate(sizeof(Shader));

    platform_zero_memory(opengl_shader, sizeof(Shader));

    opengl_shader->name = config->name;
    opengl_shader->vt.shader_create = shader_opengl_create;
    opengl_shader->vt.shader_destroy = shader_opengl_destroy;
    opengl_shader->vt.shader_bind = shader_opengl_bind;
    opengl_shader->vt.shader_unbind = shader_opengl_unbind;

    return opengl_shader;
}
