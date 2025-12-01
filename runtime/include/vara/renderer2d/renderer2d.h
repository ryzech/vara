#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

typedef struct Renderer2D Renderer2D;
typedef struct Renderer2DConfig Renderer2DConfig;

struct RenderPass;
struct Buffer;
struct Shader;
struct Texture;

struct Renderer2DConfig {
    u32 max_vertices;
    u32 max_indices;
};

struct Renderer2D {
    Vertex* vertices;
    struct Buffer* vertex_buffer;
    struct Buffer* index_buffer;
    struct Shader* shader;
    struct Texture* textures[16];
    u32* indices;
    u32 vertex_count;
    u32 max_vertices;
    u32 index_count;
    u32 max_indices;
    u32 texture_count;
};

Renderer2D* renderer2d_create(const Renderer2DConfig* config);
void renderer2d_destroy(Renderer2D* renderer);

void renderer2d_begin(Renderer2D* renderer);
void renderer2d_end(Renderer2D* renderer, struct RenderPass* pass);

void renderer2d_draw_rect(Renderer2D* renderer, Rect rect, Vector4 color);
void renderer2d_draw_rect_texture(
    Renderer2D* renderer, Rect rect, struct Texture* texture, Vector4 tint
);