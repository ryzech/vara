#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/texture.h"
#include "vendor/stb/stb_image.h"

Texture* texture_create(const TextureConfig* config) {
    Texture* texture = platform_allocate(sizeof(Texture));
    platform_zero_memory(texture, sizeof(Texture));

    texture->width = config->width;
    texture->height = config->height;
    texture->samples = config->samples;
    texture->format = config->format;
    texture->filter = config->filter;

    const RendererBackend* backend = renderer_backend_get();
    if (!backend->texture.create(texture, config)) {
        texture_destroy(texture);
        return NULL;
    }

    return texture;
}

Texture* texture_load_file(const TextureConfig* config, const char* file) {
    if (!file) {
        return NULL;
    }

    i32 width, height, channels;
    unsigned char* data = stbi_load(file, &width, &height, &channels, 0);
    if (!data) {
        ERROR("stbi failed to load image('%s')\nError: %s", file, stbi_failure_reason());
        return NULL;
    }

    TextureFormat format;
    switch (channels) {
        case 3:
            format = TEXTURE_FORMAT_RGB;
            break;
        case 4:
            format = TEXTURE_FORMAT_RGBA;
            break;
        default:
            stbi_image_free(data);
            return NULL;
    }

    const TextureConfig new_config = {
        .filter = config->filter,
        .format = format,
        .width = (u32)width,
        .height = (u32)height,
    };

    Texture* texture = texture_create(&new_config);
    texture_set_data(texture, data, width * height * channels);
    stbi_image_free(data);

    return texture;
}

void texture_destroy(Texture* texture) {
    if (texture) {
        const RendererBackend* backend = renderer_backend_get();
        backend->texture.destroy(texture);
        platform_free(texture);
    }
}

void texture_bind(Texture* texture, u32 slot) {
    if (texture) {
        const RendererBackend* backend = renderer_backend_get();
        backend->texture.bind(texture, slot);
    }
}

void texture_unbind(Texture* texture) {
    if (texture) {
        const RendererBackend* backend = renderer_backend_get();
        backend->texture.unbind(texture);
    }
}

void texture_set_data(Texture* texture, void* data, size_t size) {
    if (!texture || !data) {
        return;
    }

    const RendererBackend* backend = renderer_backend_get();
    backend->texture.set_data(texture, data, size);
}

u32 texture_get_id(Texture* texture) {
    if (texture) {
        const RendererBackend* backend = renderer_backend_get();
        return backend->texture.get_id(texture);
    }

    return 0;
}