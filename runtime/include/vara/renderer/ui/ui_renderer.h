#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

typedef struct UIRenderer UIRenderer;
typedef struct UIRendererConfig UIRendererConfig;
typedef struct UIDrawCommand UIDrawCommand;
typedef struct UIVertex UIVertex;

struct Buffer;
struct Shader;
struct Texture;

struct UIRendererConfig {
    u32 max_vertices;
    u32 max_indices;
};

struct UIVertex {
    Vector2 position;
    Vector2 uv;
    Vector4 color;
    Vector4 clip_rect;
    f32 corner_radius;
};

struct UIDrawCommand {
    struct Shader* shader;
    struct Texture* texture;
    UIVertex vertices[4];
    u32 indices[6];
    i32 z_index;
};