#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/renderer/shader.h>

#include "vara/renderer/shader_compiler_opengl.h"

static GLenum stage_to_gl_type(ShaderStage stage) {
    switch (stage) {
        case SHADER_STAGE_VERTEX:
            return GL_VERTEX_SHADER;
        case SHADER_STAGE_FRAGMENT:
            return GL_FRAGMENT_SHADER;
        case SHADER_STAGE_COMPUTE:
            return GL_COMPUTE_SHADER;
        default:
            return 0;
    }
}

static GLuint compile_shader_stage(GLenum type, const char* source, const char* stage_name) {
    GLuint shader = glCreateShader(type);
    if (!shader) {
        ERROR("Failed to create shader object for stage named('%s')", stage_name);
        return 0;
    }

    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar info_log[1024];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        ERROR("Shader compilation failed for stage named('%s'):\n%s", stage_name, info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint shader_compiler_opengl_compile(const ShaderConfig* config) {
    if (!config || !config->stage_count || !config->stages) {
        return 0;
    }

    DEBUG("Compiling shader program named('%s')", config->name);

    GLuint program = glCreateProgram();
    if (!program) {
        ERROR("Failed to create OpenGL program");
        return 0;
    }

    GLuint compiled_shaders[16];
    u32 shader_count = 0;

    for (u16 i = 0; i < config->stage_count; i++) {
        const ShaderSource* src = &config->stages[i];
        if (!src->source) {
            return 0;
        }

        const char* stage_name = shader_stage_name(src->stage);
        GLenum gl_type = stage_to_gl_type(src->stage);
        if (!gl_type) {
            return 0;
        }

        GLuint shader = compile_shader_stage(gl_type, src->source, stage_name);
        if (!shader) {
            return 0;
        }

        glAttachShader(program, shader);
        compiled_shaders[shader_count++] = shader;
    }

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar info_log[1024];
        glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
        ERROR("Program linking failed for shader named('%s'):\n%s", config->name, info_log);
        glDeleteProgram(program);
        return 0;
    }

    for (u32 i = 0; i < shader_count; i++) {
        glDetachShader(program, compiled_shaders[i]);
        glDeleteShader(compiled_shaders[i]);
    }

    return program;
}

void shader_compiler_opengl_delete(GLuint program) {
    if (program) {
        glDeleteProgram(program);
    }
}
