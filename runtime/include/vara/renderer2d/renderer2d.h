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

struct Renderer2DConfig {
    u32 max_vertices;
    u32 max_indices;
};

struct Renderer2D {
    Vertex* vertices;
    struct Buffer* vertex_buffer;
    struct Buffer* index_buffer;
    struct Texture* textures[RENDERER2D_MAX_TEXTURES];
    DrawCommand* commands[RENDERER2D_MAX_COMMANDS];
    u32* indices;
    u32 vertex_count;
    u32 max_vertices;
    u32 index_count;
    u32 max_indices;
    u32 texture_count;
    u32 command_count;
    u32 draw_calls;
};

struct DrawCommand {
    Vector4 color;
    struct Shader* shader;
    struct Texture* texture;
    Vertex* vertices;
    u32* indices;
    u32 vertex_count;
    u32 index_count;
    f32 z_index;
};

b8 renderer2d_create(const Renderer2DConfig* config);
void renderer2d_destroy(void);

void renderer2d_begin(void);
void renderer2d_end(void);

void renderer2d_draw_rect(Rect rect, Vector4 color);
void renderer2d_draw_sprite(Rect rect, struct Texture* texture, Vector4 tint);
void renderer2d_draw_text(const char* text, struct Font* font, Vector2 position, Vector4 color);