#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/renderer.h"
#include "vara/renderer/shader.h"

extern Shader* shader_opengl_init(const ShaderConfig* config);

Shader* shader_create(const ShaderConfig* config) {
    Shader* shader = NULL;
    RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                shader = shader_opengl_init(config);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(instance->renderer_type)
                );
                return NULL;
        }
    }

    if (!shader->vt.shader_create(shader, config)) {
        ERROR("Failed to create shader named('%s')", config->name);
        platform_free(shader);
        return NULL;
    }

    return shader;
}

void shader_destroy(Shader* shader) {
    if (shader && shader->vt.shader_destroy) {
        shader->vt.shader_destroy(shader);
        platform_free(shader);
    }
}

void shader_bind(Shader* shader) {
    if (shader && shader->vt.shader_bind) {
        shader->vt.shader_bind(shader);
    }
}

void shader_unbind(Shader* shader) {
    if (shader && shader->vt.shader_unbind) {
        shader->vt.shader_unbind(shader);
    }
}

void shader_set_mat4(Shader* shader, const char* name, const Matrix4* matrix) {
    if (shader && shader->vt.shader_set_mat4) {
        shader->vt.shader_set_mat4(shader, name, matrix);
    }
}

void shader_dispatch(Shader* shader, i16 x, i16 y, i16 z) {
    if (shader && shader->vt.shader_dispatch) {
        shader->vt.shader_dispatch(shader, x, y, z);
    }
}
