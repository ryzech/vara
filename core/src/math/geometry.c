#include "vara/core/math/geometry.h"
#include "vara/core/math/math.h"
#include "vara/core/platform/platform.h"

Geometry2D geometry_generate_quad(Vector2 size, Vector2 uv0, Vector2 uv1, Vector4 color) {
    Geometry2D data;
    data.vertex_count = 4;
    data.index_count = 6;

    data.vertices = platform_allocate(sizeof(Vertex) * data.vertex_count);
    data.indices = platform_allocate(sizeof(u32) * data.index_count);

    data.vertices[0] = (Vertex){
        .position = {0.0f, 0.0f, 0.0f},
        .color = color,
        .tex_coord = {uv0.x, uv0.y},
    };
    data.vertices[1] = (Vertex){
        .position = {size.x, 0.0f, 0.0f},
        .color = color,
        .tex_coord = {uv1.x, uv0.y},
    };
    data.vertices[2] = (Vertex){
        .position = {size.x, size.y, 0.0f},
        .color = color,
        .tex_coord = {uv1.x, uv1.y},
    };
    data.vertices[3] = (Vertex){
        .position = {0.0f, size.y, 0.0f},
        .color = color,
        .tex_coord = {uv0.x, uv1.y},
    };

    data.indices[0] = 0;
    data.indices[1] = 1;
    data.indices[2] = 2;
    data.indices[3] = 2;
    data.indices[4] = 3;
    data.indices[5] = 0;

    return data;
}

void geometry_transform(Geometry2D* geometry, const Matrix4* matrix) {
    for (u32 i = 0; i < geometry->vertex_count; i++) {
        geometry->vertices[i].position = mat4_mul_vec3(*matrix, geometry->vertices[i].position);
    }
}

void geometry_destroy(Geometry2D* geometry) {
    if (geometry->vertices) {
        platform_free(geometry->vertices);
        geometry->vertices = NULL;
    }
    if (geometry->indices) {
        platform_free(geometry->indices);
        geometry->indices = NULL;
    }
    geometry->vertex_count = 0;
    geometry->index_count = 0;
}
