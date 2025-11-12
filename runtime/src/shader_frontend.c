#include "vara/renderer/renderer.h"
#include "vara/renderer/shader.h"

#include <stdlib.h>

#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

extern Shader* shader_opengl_init(ShaderConfig* config);

Shader* shader_create(RendererInstance* instance, ShaderConfig* config) {
    Shader* shader = NULL;
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

    if (!shader->vt.shader_create(config)) {
        ERROR("Failed to create shader: %s", config->name);
        platform_free(shader);
        return NULL;
    }

    return shader;
}

void shader_destroy(Shader* shader) {
    if (shader && shader->vt.shader_destroy) {
        shader->vt.shader_destroy();
        platform_free(shader);
    }
}

void shader_bind(Shader* shader) {
    if (shader && shader->vt.shader_bind) {
        shader->vt.shader_bind();
    }
}

void shader_unbind(Shader* shader) {
    if (shader && shader->vt.shader_unbind) {
        shader->vt.shader_unbind();
    }
}
