#include <glad/gl.h>

#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/renderer/renderer.h>
#include <vara/renderer/shader.h>

typedef struct OpenGLShaderState {
    u32 shader_program;
} OpenGLShaderState;

static OpenGLShaderState shader_state;

static b8 shader_opengl_create(ShaderConfig* config) {
    shader_state.shader_program = glCreateProgram();
    glLinkProgram(shader_state.shader_program);
    return true;
}

static void shader_opengl_destroy(void) {
    if (shader_state.shader_program) {
        glDeleteProgram(shader_state.shader_program);
        shader_state.shader_program = 0;
    }
}

static void shader_opengl_bind(void) {
    glUseProgram(shader_state.shader_program);
}

static void shader_opengl_unbind(void) {
    glUseProgram(0);
}

Shader* shader_opengl_init(ShaderConfig* config) {
    Shader* opengl_shader = platform_allocate(sizeof(Shader));

    opengl_shader->name = config->name;
    opengl_shader->vt.shader_create = shader_opengl_create;
    opengl_shader->vt.shader_destroy = shader_opengl_destroy;
    opengl_shader->vt.shader_bind = shader_opengl_bind;
    opengl_shader->vt.shader_unbind = shader_opengl_unbind;

    DEBUG("Creating Shader named('%s')", opengl_shader->name);
    return opengl_shader;
}
