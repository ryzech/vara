#pragma once

#include <vara/core/defines.h>

// Forward declarations, in case they need access to each other.
typedef enum BufferType BufferType;
typedef enum BufferUsage BufferUsage;
typedef enum VertexAttributeType VertexAttributeType;
typedef struct VertexAttribute VertexAttribute;
typedef struct VertexLayout VertexLayout;
typedef struct BufferConfig BufferConfig;
typedef struct BufferVT BufferVT;
typedef struct Buffer Buffer;

enum BufferType {
    BUFFER_TYPE_VERTEX,
    BUFFER_TYPE_INDEX,
    BUFFER_TYPE_UNIFORM
};

enum BufferUsage {
    BUFFER_USAGE_STATIC,
    BUFFER_USAGE_DYNAMIC,
    BUFFER_USAGE_STREAM
};

// Should this be called IVEC4 and VEC4?
enum VertexAttributeType {
    VERTEX_ATTRIBUTE_FLOAT,
    VERTEX_ATTRIBUTE_FLOAT2,
    VERTEX_ATTRIBUTE_FLOAT3,
    VERTEX_ATTRIBUTE_FLOAT4,
    VERTEX_ATTRIBUTE_INT,
    VERTEX_ATTRIBUTE_INT2,
    VERTEX_ATTRIBUTE_INT3,
    VERTEX_ATTRIBUTE_INT4
};

struct VertexAttribute {
    VertexAttributeType type;
    // Should this be a size_t?
    u32 offset;
    u32 location;
    b8 normalized;
};

struct VertexLayout {
    VertexAttribute* attributes;
    size_t attribute_count;
    size_t stride;
};

struct BufferConfig {
    BufferType type;
    BufferUsage usage;
    VertexLayout* layout;
    size_t size;
    const void* data;
};

struct BufferVT {
    b8 (*buffer_create)(Buffer* buffer, const BufferConfig* config);
    void (*buffer_destroy)(Buffer* buffer);
    void (*buffer_bind)(Buffer* buffer);
    void (*buffer_unbind)(Buffer* buffer);
};

struct Buffer {
    BufferType type;
    BufferUsage usage;
    VertexLayout layout;
    BufferVT vt;
    size_t element_count;
    size_t size;
    void* backend_data;
};

Buffer* buffer_create(const BufferConfig* config);
void buffer_destroy(Buffer* buffer);

void buffer_bind(Buffer* buffer);
void buffer_unbind(Buffer* buffer);
