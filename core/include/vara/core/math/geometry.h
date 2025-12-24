#pragma once

#include "types.h"

typedef struct Geometry2D Geometry2D;

struct Geometry2D {
    Vertex* vertices;
    u32* indices;
    u32 vertex_count;
    u32 index_count;
};

Geometry2D geometry_generate_quad(Vector2 size, Vector2 uv0, Vector2 uv1, Vector4 color);
void geometry_transform(Geometry2D* geometry, const Matrix4* matrix);
void geometry_destroy(Geometry2D* geometry);