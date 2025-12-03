#pragma once

#include "vara/renderer/texture.h"

typedef struct TextureBackendVT TextureBackendVT;

struct TextureBackendVT {
    b8 (*create)(Texture* texture, const TextureConfig* config);
    void (*destroy)(Texture* texture);
    void (*bind)(Texture* texture, u32 slot);
    void (*unbind)(Texture* texture);
    void (*set_data)(Texture* texture, void* data, size_t size);
    u32 (*get_id)(Texture* texture);
};