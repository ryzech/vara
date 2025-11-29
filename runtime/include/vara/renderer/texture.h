#pragma once

#include <vara/core/defines.h>

typedef enum TextureFormat TextureFormat;
typedef enum TextureFilter TextureFilter;
typedef enum TextureWrap TextureWrap;
typedef struct TextureConfig TextureConfig;
typedef struct Texture Texture;
typedef struct TextureVT TextureVT;

enum TextureFormat {
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
    TextureFormat format;
    TextureFilter filter;
};

struct TextureVT {
    b8 (*texture_create)(Texture* texture, const TextureConfig* config);
    void (*texture_destroy)(Texture* texture);
    void (*texture_bind)(Texture* texture, u32 slot);
    void (*texture_unbind)(Texture* texture);
    void (*texture_set_data)(Texture* texture, void* data, size_t size);
    u32 (*texture_get_id)(Texture* texture);
};

struct Texture {
    TextureVT vt;
    u32 width, height;
    void* backend_data;
    TextureFormat format;
    TextureFilter filter;
};

Texture* texture_create(const TextureConfig* config);
Texture* texture_load_file(const TextureConfig* config, const char* file);
void texture_destroy(Texture* texture);

void texture_bind(Texture* texture, u32 slot);
void texture_unbind(Texture* texture);
void texture_set_data(Texture* texture, void* data, size_t size);
u32 texture_get_id(Texture* texture);
