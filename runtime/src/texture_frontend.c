#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/renderer.h"
#include "vara/renderer/texture.h"

extern void texture_opengl_init(Texture* texture);

Texture* texture_create(const TextureConfig* config) {
    Texture* texture = platform_allocate(sizeof(Texture));
    platform_zero_memory(texture, sizeof(Texture));

    texture->width = config->width;
    texture->height = config->height;
    texture->format = config->format;
    texture->filter = config->filter;

    const RendererInstance* instance = renderer_get_instance();
    switch (instance->renderer_type) {
        case RENDERER_TYPE_OPENGL:
            texture_opengl_init(texture);
            break;
        default:
            ERROR(
                "Unsupported graphics type: %s", renderer_type_to_string(instance->renderer_type)
            );
            return NULL;
    }

    if (!texture->vt.texture_create(texture, config)) {
        texture_destroy(texture);
        return NULL;
    }

    return texture;
}

void texture_destroy(Texture* texture) {
    if (texture && texture->vt.texture_destroy) {
        texture->vt.texture_destroy(texture);
        platform_free(texture);
    }
}

void texture_bind(Texture* texture, u32 slot) {
    if (texture && texture->vt.texture_bind) {
        texture->vt.texture_bind(texture, slot);
    }
}

void texture_unbind(Texture* texture) {
    if (texture && texture->vt.texture_unbind) {
        texture->vt.texture_unbind(texture);
    }
}

void texture_set_data(Texture* texture, void* data, size_t size) {
    if (!texture || !data) {
        return;
    }

    if (texture->vt.texture_set_data) {
        texture->vt.texture_set_data(texture, data, size);
    }
}

u32 texture_get_id(Texture* texture) {
    if (texture && texture->vt.texture_get_id) {
        return texture->vt.texture_get_id(texture);
    }

    return 0;
}