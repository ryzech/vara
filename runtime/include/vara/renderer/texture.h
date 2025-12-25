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
    TEXTURE_FORMAT_R,
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_RGBA,
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
};

struct Texture {
    u32 width, height;
    u32 samples;
    void* backend_data;
    TextureFormat format;
    TextureFilter filter;
    struct RendererBackend* backend;
};

Texture* texture_create(struct Renderer* renderer, const TextureConfig* config);
Texture* texture_load_file(
    struct Renderer* renderer, const TextureConfig* config, const char* file
);
void texture_destroy(Texture* texture);

void texture_bind(Texture* texture, u32 slot);
void texture_unbind(Texture* texture);
void texture_set_data(Texture* texture, void* data, size_t size);
u32 texture_get_id(Texture* texture);
