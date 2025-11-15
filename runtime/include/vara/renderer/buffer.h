#pragma once

#include <vara/core/defines.h>

#include "vara/renderer/renderer.h"

typedef enum BufferType {
    BUFFER_TYPE_VERTEX,
    BUFFER_TYPE_INDEX,
    BUFFER_TYPE_UNIFORM
} BufferType;

typedef enum BufferUsage {
    BUFFER_USAGE_STATIC,
    BUFFER_USAGE_DYNAMIC,
    BUFFER_USAGE_STREAM
} BufferUsage;

// Should this be called IVEC4 and VEC4?
typedef enum VertexAttributeType {
    VERTEX_ATTRIBUTE_FLOAT,
    VERTEX_ATTRIBUTE_FLOAT2,
    VERTEX_ATTRIBUTE_FLOAT3,
    VERTEX_ATTRIBUTE_FLOAT4,
    VERTEX_ATTRIBUTE_INT,
    VERTEX_ATTRIBUTE_INT2,
    VERTEX_ATTRIBUTE_INT3,
    VERTEX_ATTRIBUTE_INT4
} VertexAttributeType;

typedef struct VertexAttribute {
    u32 location;
    VertexAttributeType type;
    u32 offset;
    b8 normalized;
} VertexAttribute;

typedef struct VertexLayout {
    VertexAttribute* attributes;
    u32 attribute_count;
    u32 stride;
} VertexLayout;

typedef struct BufferConfig {
    BufferType type;
    BufferUsage usage;
    VertexLayout* layout;
    const void* data;
    u64 size;
} BufferConfig;

struct Buffer;

typedef struct BufferVT {
    b8 (*buffer_create)(struct Buffer* buffer, const BufferConfig* config);
    void (*buffer_destroy)(struct Buffer* buffer);
    void (*buffer_bind)(struct Buffer* buffer);
    void (*buffer_unbind)(struct Buffer* buffer);
} BufferVT;

typedef struct Buffer {
    BufferType type;
    BufferUsage usage;
    u64 size;
    VertexLayout* layout;
    u32 element_count;
    BufferVT vt;
    void* backend_data;
} Buffer;

Buffer* buffer_create(RendererInstance* instance, const BufferConfig* config);

void buffer_destroy(Buffer* buffer);

void buffer_bind(Buffer* buffer);

void buffer_unbind(Buffer* buffer);
