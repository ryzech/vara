#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer/shader.h"

Shader* shader_create(const ShaderConfig* config) {
    Shader* shader = platform_allocate(sizeof(Shader));
    platform_zero_memory(shader, sizeof(Shader));

    shader->name = config->name;

    const RendererBackend* backend = renderer_backend_get();
    if (!backend->shader.create(shader, config)) {
        ERROR("Failed to create shader named('%s')", config->name);
        shader_destroy(shader);
        return NULL;
    }

    return shader;
}

void shader_destroy(Shader* shader) {
    if (shader) {
        const RendererBackend* backend = renderer_backend_get();
        backend->shader.destroy(shader);
        platform_free(shader);
    }
}

void shader_bind(Shader* shader) {
    if (shader) {
        const RendererBackend* backend = renderer_backend_get();
        backend->shader.bind(shader);
    }
}

void shader_unbind(Shader* shader) {
    if (shader) {
        const RendererBackend* backend = renderer_backend_get();
        backend->shader.unbind(shader);
    }
}

void shader_set_mat4(Shader* shader, const char* name, Matrix4 matrix) {
    if (shader) {
        const RendererBackend* backend = renderer_backend_get();
        backend->shader.set_mat4(shader, name, matrix);
    }
}

void shader_set_int_array(Shader* shader, const char* name, const i32* array, u32 count) {
    if (shader) {
        const RendererBackend* backend = renderer_backend_get();
        backend->shader.set_int_array(shader, name, array, count);
    }
}

void shader_dispatch(Shader* shader, const i16 x, const i16 y, const i16 z) {
    if (shader) {
        const RendererBackend* backend = renderer_backend_get();
        backend->shader.dispatch(shader, x, y, z);
    }
}
