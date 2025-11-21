#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/renderer/shader.h>

#include "vara/renderer/shader_compiler_opengl.h"

static u32 stage_to_gl_type(ShaderStage stage, const char** out_name) {
    switch (stage) {
        case SHADER_STAGE_VERTEX:
            *out_name = "vertex_shader";
            return GL_VERTEX_SHADER;
        case SHADER_STAGE_FRAGMENT:
            *out_name = "fragment_shader";
            return GL_FRAGMENT_SHADER;
        case SHADER_STAGE_COMPUTE:
            *out_name = "compute_shader";
            return GL_COMPUTE_SHADER;
        default:
            *out_name = "unknown_shader";
            return 0;
    }
}

static u32 compile_shader_stage(
    u32 type, const char* source, const char* stage_name
) {
    u32 shader = glCreateShader(type);
    if (!shader) {
        ERROR(
            "Failed to create shader object for stage named('%s')", stage_name
        );
        return 0;
    }

    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, sizeof(info_log), 0, info_log);
        ERROR(
            "Shader compilation failed for stage named('%s'):\n%s",
            stage_name,
            info_log
        );
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

u32 shader_compiler_opengl_compile(const ShaderConfig* config) {
    if (!config) {
        return 0;
    }

    if (!config->stage_count || !config->stages) {
        return 0;
    }

    DEBUG("Compiling shader program named('%s')", config->name);

    u32 program = glCreateProgram();
    if (!program) {
        ERROR("Failed to create OpenGL program");
        return 0;
    }

    u32 compiled_shaders[16];
    u32 shader_count = 0;

    for (u16 i = 0; i < config->stage_count; i++) {
        const ShaderSource* src = &config->stages[i];
        if (!src->source) {
            return 0;
        }

        const char* stage_name;
        u32 gl_type = stage_to_gl_type(src->stage, &stage_name);
        if (!gl_type) {
            return 0;
        }

        u32 shader = compile_shader_stage(gl_type, src->source, stage_name);
        if (!shader) {
            return 0;
        }

        glAttachShader(program, shader);
        compiled_shaders[shader_count++] = shader;
    }

    glLinkProgram(program);

    i32 success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[1024];
        glGetProgramInfoLog(program, sizeof(info_log), 0, info_log);
        ERROR(
            "Program linking failed for shader named('%s'):\n%s",
            config->name,
            info_log
        );
        return 0;
    }

    for (u32 i = 0; i < shader_count; i++) {
        glDetachShader(program, compiled_shaders[i]);
        glDeleteShader(compiled_shaders[i]);
    }

    return program;
}

void shader_compiler_opengl_delete(u32 program) {
    if (program) {
        glDeleteProgram(program);
    }
}
