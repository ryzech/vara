#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>
#include <spirv_cross_c.h>
#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/util/string.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/internal/shader/shader_compiler_internal.h"

static b8 glslang_initialized;

static glslang_stage_t stage_to_glslang(ShaderStage stage) {
    switch (stage) {
        case SHADER_STAGE_VERTEX:
            return GLSLANG_STAGE_VERTEX;
        case SHADER_STAGE_FRAGMENT:
            return GLSLANG_STAGE_FRAGMENT;
        case SHADER_STAGE_COMPUTE:
            return GLSLANG_STAGE_COMPUTE;
        default:
            return GLSLANG_STAGE_VERTEX;
    }
}

static void* compile_glsl(ShaderSource* source, u32* out_size) {
    const glslang_input_t input = {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = stage_to_glslang(source->stage),
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_2,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_6,
        .code = source->source,
        .default_version = 410,
        .default_profile = GLSLANG_NO_PROFILE,
        .messages = GLSLANG_MSG_DEFAULT_BIT | GLSLANG_MSG_RELAXED_ERRORS_BIT,
        .resource = glslang_default_resource()
    };
    glslang_shader_t* shader = glslang_shader_create(&input);

    if (!glslang_shader_preprocess(shader, &input)) {
        ERROR("GLSL preprocessing failed for stage named('%s')", shader_stage_name(source->stage));
        ERROR("%s", glslang_shader_get_info_log(shader));
        glslang_shader_delete(shader);
        return NULL;
    }

    if (!glslang_shader_parse(shader, &input)) {
        ERROR("GLSL parsing failed for stage named('%s')", shader_stage_name(source->stage));
        ERROR("%s", glslang_shader_get_info_log(shader));
        glslang_shader_delete(shader);
        return NULL;
    }

    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT)) {
        ERROR("SPIR-V linking failed for stage named('%s')", shader_stage_name(source->stage));
        ERROR("%s", glslang_program_get_info_log(program))
        glslang_program_delete(program);
        glslang_shader_delete(shader);
        return NULL;
    }

    glslang_program_SPIRV_generate(program, stage_to_glslang(source->stage));
    const size_t spirv_size = glslang_program_SPIRV_get_size(program);
    *out_size = spirv_size * sizeof(u32);

    void* bytecode = vara_allocate(*out_size);
    vara_copy_memory(bytecode, glslang_program_SPIRV_get_ptr(program), *out_size);

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return bytecode;
}

static char* compile_spirv_to_glsl(const void* bytecode, u32 bytecode_size, u32 glsl_version) {
    spvc_context context;
    spvc_context_create(&context);
    spvc_parsed_ir parsed;
    if (spvc_context_parse_spirv(context, bytecode, bytecode_size / sizeof(u32), &parsed)
        != SPVC_SUCCESS) {
        spvc_context_destroy(context);
        return NULL;
    }

    spvc_compiler compiler;
    spvc_context_create_compiler(
        context, SPVC_BACKEND_GLSL, parsed, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler
    );
    spvc_compiler_options options;
    spvc_compiler_create_compiler_options(compiler, &options);
    spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION, glsl_version);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES, false);
    spvc_compiler_install_compiler_options(compiler, options);

    const char* result;
    if (spvc_compiler_compile(compiler, &result) != SPVC_SUCCESS) {
        spvc_context_destroy(context);
        return NULL;
    }

    const size_t length = string_length(result);
    char* output = vara_allocate(length + 1);
    vara_copy_memory(output, result, length + 1);

    spvc_context_destroy(context);
    return output;
}

CompiledShader* shader_compiler_compile(
    const ShaderConfig* config, struct RendererBackend* backend
) {
    if (!glslang_initialized) {
        glslang_initialize_process();
        glslang_initialized = true;
    }

    CompiledShader* compiled = vara_allocate(sizeof(CompiledShader));
    vara_zero_memory(compiled, sizeof(CompiledShader));

    compiled->name = config->name;
    compiled->stage_count = config->stage_count;
    compiled->stages = vara_allocate(sizeof(CompiledShaderStage) * config->stage_count);

    for (u32 i = 0; i < config->stage_count; i++) {
        ShaderSource* source = &config->stages[i];
        CompiledShaderStage* stage = &compiled->stages[i];
        stage->stage = source->stage;

        u32 bytecode_size;
        void* bytecode = compile_glsl(source, &bytecode_size);
        if (!bytecode) {
            shader_compiler_release(compiled);
            return NULL;
        }

        switch (backend->type) {
            case RENDERER_TYPE_OPENGL: {
                stage->bytecode = compile_spirv_to_glsl(bytecode, bytecode_size, 410);
                vara_free(bytecode, bytecode_size);
                break;
            }
            default: {
                vara_free(bytecode, bytecode_size);
                break;
            }
        }
    }

    return compiled;
}

void shader_compiler_release(CompiledShader* compiled) {
    if (!compiled) {
        return;
    }

    for (u32 i = 0; i < compiled->stage_count; i++) {
        CompiledShaderStage* stage = &compiled->stages[i];
        if (stage->bytecode) {
            size_t size = stage->bytecode_size;
            if (size == 0) {
                size = string_length((const char*)stage->bytecode) + 1;
            }
            vara_free(stage->bytecode, size);
            stage->bytecode = NULL;
        }
    }

    if (compiled->stages) {
        vara_free(compiled->stages, sizeof(CompiledShaderStage) * compiled->stage_count);
        compiled->stages = NULL;
    }

    vara_free(compiled, sizeof(CompiledShader));
}
