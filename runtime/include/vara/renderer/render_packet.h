#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

typedef struct RenderPacket RenderPacket;

struct Shader;
struct Buffer;
struct Texture;

struct RenderPacket {
    struct Shader* shader;
    struct Buffer* vertex_buffer;
    struct Buffer* index_buffer;
    struct Texture* textures[16];
    u32 texture_count;
    Matrix4 model;
    u32 index_count;
    u32 first_index;
    u64 sort_key;
};