#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

typedef struct RenderPacket RenderPacket;

struct Buffer;
struct Material;
struct RenderPipeline;

struct RenderPacket {
    struct RenderPipeline* pipeline;
    struct Material* material;
    struct Buffer* vertex_buffer;
    struct Buffer* index_buffer;
    u32 index_count, vertex_count;
    u32 first_index, first_vertex;
    u64 sort_key;
};