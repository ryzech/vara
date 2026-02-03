#pragma once

#include <vara/core/defines.h>

#include "vara/renderer/render_types.h"

// Forward declarations, in case they need access to each other.
typedef enum BufferType BufferType;
typedef enum BufferUsage BufferUsage;
typedef struct VertexAttribute VertexAttribute;
typedef struct VertexLayout VertexLayout;
typedef struct BufferConfig BufferConfig;
typedef struct Buffer Buffer;

struct Renderer;
struct RendererBackend;

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

struct VertexAttribute {
    VertexAttributeType type;
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
    VertexLayout* layout;
    size_t size;
    const void* data;
    BufferType type;
    BufferUsage usage;
    u32 binding;
};

struct Buffer {
    VertexLayout layout;
    size_t element_count;
    size_t size;
    void* backend_data;
    BufferType type;
    BufferUsage usage;
    u32 binding;
    struct RendererBackend* backend;
};

Buffer* _buffer_create(struct RendererBackend* backend, const BufferConfig* config);
Buffer* buffer_create(struct Renderer* renderer, const BufferConfig* config);
void buffer_destroy(Buffer* buffer);

void buffer_bind(Buffer* buffer);
void buffer_unbind(Buffer* buffer);
void buffer_set_data(Buffer* buffer, const void* data, size_t size, size_t offset);
