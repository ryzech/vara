#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

#define RENDERER2D_MAX_TEXTURES 16
#define RENDERER2D_MAX_COMMANDS 1024

typedef struct Renderer2D Renderer2D;
typedef struct Renderer2DConfig Renderer2DConfig;
typedef struct DrawCommand DrawCommand;

struct Buffer;
struct Shader;
struct Texture;
struct Font;
struct Renderer;

struct Renderer2DConfig {
    u32 max_vertices;
    u32 max_indices;
};

struct DrawCommand {
    Vector4 color;
    struct Shader* shader;
    struct Texture* texture;
    Vertex vertices[4];
    u32 indices[6];
    u32 vertex_count;
    u32 index_count;
    i32 z_index;
    i32 submission_id;
};

struct Renderer2D {
    struct Renderer* renderer;
    Vertex* vertices;
    struct Buffer* vertex_buffer;
    struct Buffer* index_buffer;
    struct Texture* textures[RENDERER2D_MAX_TEXTURES];
    DrawCommand commands[RENDERER2D_MAX_COMMANDS];
    u32* indices;
    u32 vertex_count;
    u32 max_vertices;
    u32 index_count;
    u32 max_indices;
    u32 texture_count;
    u32 command_count;
    u32 submission_count;
    u32 draw_calls;
};

b8 renderer2d_create(struct Renderer* backend, const Renderer2DConfig* config);
void renderer2d_destroy(void);

void renderer2d_begin(struct Renderer* backend);
void renderer2d_end(void);

void renderer2d_draw_rect(const Vector2 position, const Vector2 size, Vector4 color, i32 z_index);
void renderer2d_draw_rect_matrix(const Matrix4 matrix, Vector4 color, i32 z_index);

void renderer2d_draw_sprite(
    const Vector2 position, const Vector2 size, struct Texture* texture, Vector4 tint, i32 z_index
);
void renderer2d_draw_sprite_matrix(
    const Matrix4 matrix, struct Texture* texture, Vector4 tint, i32 z_index
);

void renderer2d_draw_text(
    const Vector2 position,
    const Vector2 size,
    const char* text,
    struct Font* font,
    Vector4 color,
    i32 z_index
);
void renderer2d_draw_text_matrix(
    const Matrix4 matrix, const char* text, struct Font* font, Vector4 color, i32 z_index
);