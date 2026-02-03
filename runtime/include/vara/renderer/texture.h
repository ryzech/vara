#pragma once

#include <vara/core/defines.h>

typedef enum TextureFormat TextureFormat;
typedef enum TextureFilter TextureFilter;
typedef enum TextureWrap TextureWrap;
typedef struct TextureConfig TextureConfig;
typedef struct Texture Texture;

struct Renderer;
struct RendererBackend;

enum TextureFormat {
    TEXTURE_FORMAT_R8,
    TEXTURE_FORMAT_RG8,
    TEXTURE_FORMAT_RGB8,
    TEXTURE_FORMAT_RGBA8,
    TEXTURE_FORMAT_R16,
    TEXTURE_FORMAT_RG16,
    TEXTURE_FORMAT_RGB16,
    TEXTURE_FORMAT_RGBA16,
    TEXTURE_FORMAT_D32,
    TEXTURE_FORMAT_D24,
    TEXTURE_FORMAT_S8,
};

enum TextureFilter {
    TEXTURE_FILTER_NEAREST,
    TEXTURE_FILTER_LINEAR,
};

enum TextureWrap {
    TEXTURE_WRAP_REPEAT,
    TEXTURE_WRAP_CLAMP_TO_EDGE,
    TEXTURE_WRAP_CLAMP_TO_BORDER,
};

struct TextureConfig {
    u32 width, height;
    u32 samples;
    TextureFormat format;
    TextureFilter filter;
    TextureWrap wrap;
};

struct Texture {
    u32 width, height;
    u32 samples;
    void* backend_data;
    TextureFormat format;
    TextureFilter filter;
    TextureWrap wrap;
    struct RendererBackend* backend;
};

Texture* _texture_create(struct RendererBackend* backend, const TextureConfig* config);
Texture* texture_create(struct Renderer* renderer, const TextureConfig* config);
Texture* texture_load_file(
    struct Renderer* renderer, const TextureConfig* config, const char* file
);
void texture_destroy(Texture* texture);

void texture_bind(Texture* texture, u32 slot);
void texture_unbind(Texture* texture);
void texture_set_data(Texture* texture, void* data, size_t size);
