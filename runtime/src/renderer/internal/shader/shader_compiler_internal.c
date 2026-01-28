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

static VertexAttributeType attribute_type_from_reflection(spvc_type type) {
    spvc_basetype base = spvc_type_get_basetype(type);
    u32 vec_size = spvc_type_get_vector_size(type);
    switch (base) {
        case SPVC_BASETYPE_FP32: {
            switch (vec_size) {
                case 1:
                    return VERTEX_ATTRIBUTE_FLOAT;
                case 2:
                    return VERTEX_ATTRIBUTE_FLOAT2;
                case 3:
                    return VERTEX_ATTRIBUTE_FLOAT3;
                case 4:
                    return VERTEX_ATTRIBUTE_FLOAT4;
                default:
                    return VERTEX_ATTRIBUTE_FLOAT;
            }
        }
        case SPVC_BASETYPE_INT32: {
            switch (vec_size) {
                case 1:
                    return VERTEX_ATTRIBUTE_INT;
                case 2:
                    return VERTEX_ATTRIBUTE_INT2;
                case 3:
                    return VERTEX_ATTRIBUTE_INT3;
                case 4:
                    return VERTEX_ATTRIBUTE_INT4;
                default:
                    return VERTEX_ATTRIBUTE_INT;
            }
        }
        default:
            return VERTEX_ATTRIBUTE_FLOAT;
    }
}

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
        .client = GLSLANG_CLIENT_OPENGL,
        .client_version = GLSLANG_TARGET_OPENGL_450,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_6,
        .code = source->source,
        .default_version = 450,
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

static void reflect_spirv_stage(
    const void* bytecode, u32 bytecode_size, ShaderStage stage, ReflectedShaderStage* out
) {
    vara_zero_memory(out, sizeof(*out));
    out->stage = stage;

    spvc_context context;
    spvc_context_create(&context);
    spvc_parsed_ir parsed;
    if (spvc_context_parse_spirv(context, bytecode, bytecode_size / sizeof(u32), &parsed)
        != SPVC_SUCCESS) {
        spvc_context_destroy(context);
        return;
    }

    spvc_compiler compiler;
    spvc_context_create_compiler(
        context, SPVC_BACKEND_NONE, parsed, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler
    );

    spvc_resources resources;
    spvc_compiler_create_shader_resources(compiler, &resources);

    {
        const spvc_reflected_resource* resource;
        size_t count;
        spvc_resources_get_resource_list_for_type(
            resources, SPVC_RESOURCE_TYPE_STAGE_INPUT, &resource, &count
        );

        for (u32 i = 0; i < count; i++) {
            if (out->vertex_attribute_count >= 32) {
                WARN("Shader has more Vertex Attributes than are allowed (max: 32).");
                return;
            }
            const spvc_reflected_resource* input = &resource[i];
            ReflectedVertexAttribute* attribute =
                &out->vertex_attributes[out->vertex_attribute_count++];
            vara_copy_memory(attribute->name, input->name, string_length(input->name) + 1);
            attribute->location =
                spvc_compiler_get_decoration(compiler, input->id, SpvDecorationLocation);
            attribute->type = attribute_type_from_reflection(
                spvc_compiler_get_type_handle(compiler, input->type_id)
            );
        }
    }

    {
        const spvc_reflected_resource* resource;
        size_t count;
        spvc_resources_get_resource_list_for_type(
            resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &resource, &count
        );

        for (u32 i = 0; i < count; i++) {
            const spvc_reflected_resource* ubo = &resource[i];
            ReflectedDescriptor* descriptor = &out->descriptors[out->descriptor_count++];
            descriptor->type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            vara_copy_memory(descriptor->name, ubo->name, string_length(ubo->name) + 1);
            descriptor->set =
                spvc_compiler_get_decoration(compiler, ubo->id, SpvDecorationDescriptorSet);
            descriptor->binding =
                spvc_compiler_get_decoration(compiler, ubo->id, SpvDecorationBinding);
            descriptor->count = 1;
            descriptor->stage_mask = stage;
        }
    }

    {
        const spvc_reflected_resource* resource;
        size_t count;
        spvc_resources_get_resource_list_for_type(
            resources, SPVC_RESOURCE_TYPE_SAMPLED_IMAGE, &resource, &count
        );

        for (u32 i = 0; i < count; i++) {
            const spvc_reflected_resource* image = &resource[i];
            ReflectedDescriptor* descriptor = &out->descriptors[out->descriptor_count++];
            descriptor->type = DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            vara_copy_memory(descriptor->name, image->name, string_length(image->name) + 1);
            descriptor->set =
                spvc_compiler_get_decoration(compiler, image->id, SpvDecorationDescriptorSet);
            descriptor->binding =
                spvc_compiler_get_decoration(compiler, image->id, SpvDecorationBinding);
            descriptor->count = 1;
            descriptor->stage_mask = stage;
        }
    }

    spvc_context_destroy(context);
}

static void merge_reflection_stage(ReflectedShader* out, ReflectedShaderStage* stage) {
    for (u32 i = 0; i < stage->descriptor_count; i++) {
        const ReflectedDescriptor* src = &stage->descriptors[i];

        b8 found = false;
        for (u32 j = 0; j < out->descriptor_count; j++) {
            ReflectedDescriptor* dst = &out->descriptors[j];
            if (dst->set == src->set && dst->type == src->type) {
                dst->stage_mask |= src->stage_mask;
                found = true;
                break;
            }
        }

        if (!found) {
            out->descriptors[out->descriptor_count++] = *src;
        }
    }

    if (stage->stage == SHADER_STAGE_VERTEX) {
        out->vertex_attribute_count = stage->vertex_attribute_count;
        vara_copy_memory(
            out->vertex_attributes,
            stage->vertex_attributes,
            sizeof(ReflectedVertexAttribute) * stage->vertex_attribute_count
        );
    }
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

        ReflectedShaderStage stage_reflection;
        reflect_spirv_stage(bytecode, bytecode_size, stage->stage, &stage_reflection);
        merge_reflection_stage(&compiled->reflection, &stage_reflection);

        switch (backend->type) {
            case RENDERER_TYPE_OPENGL: {
                stage->bytecode = compile_spirv_to_glsl(bytecode, bytecode_size, 450);
                vara_free(bytecode, bytecode_size);
                break;
            }
            case RENDERER_TYPE_VULKAN: {
                stage->bytecode = bytecode;
                stage->bytecode_size = bytecode_size;
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
                size = string_length(stage->bytecode) + 1;
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
