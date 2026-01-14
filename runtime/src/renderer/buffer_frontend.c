#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/renderer.h"

Buffer* buffer_create(Renderer* renderer, const BufferConfig* config) {
    Buffer* buffer = vara_allocate(sizeof(Buffer));
    vara_zero_memory(buffer, sizeof(Buffer));

    buffer->type = config->type;
    buffer->usage = config->usage;
    buffer->size = config->size;
    buffer->binding = config->binding;

    // Calculate element count.
    switch (config->type) {
        case BUFFER_TYPE_VERTEX:
            buffer->element_count = config->size / config->layout->stride;
            break;
        case BUFFER_TYPE_INDEX:
            buffer->element_count = config->size / sizeof(u32);
            break;
        default:
            buffer->element_count = config->size;
            break;
    }

    if (config->layout) {
        buffer->layout.stride = config->layout->stride;
        buffer->layout.attribute_count = config->layout->attribute_count;

        if (config->layout->attribute_count > 0) {
            buffer->layout.attributes =
                vara_allocate(sizeof(VertexAttribute) * config->layout->attribute_count);
            vara_copy_memory(
                buffer->layout.attributes,
                config->layout->attributes,
                sizeof(VertexAttribute) * config->layout->attribute_count
            );
        }
    }

    RendererBackend* backend = renderer_backend_get(renderer);
    buffer->backend = backend;

    if (buffer->backend->buffer.create) {
        if (!buffer->backend->buffer.create(buffer, config)) {
            buffer_destroy(buffer);
            return NULL;
        }
    }

    return buffer;
}

void buffer_destroy(Buffer* buffer) {
    if (buffer) {
        if (buffer->backend->buffer.destroy) {
            buffer->backend->buffer.destroy(buffer);
        }

        if (buffer->layout.attributes) {
            vara_free(
                buffer->layout.attributes, sizeof(VertexAttribute) * buffer->layout.attribute_count
            );
        }
        vara_free(buffer, sizeof(Buffer));
    }
}

void buffer_bind(Buffer* buffer) {
    if (buffer) {
        buffer->backend->buffer.bind(buffer);
    }
}

void buffer_unbind(Buffer* buffer) {
    if (buffer) {
        buffer->backend->buffer.unbind(buffer);
    }
}

void buffer_set_data(Buffer* buffer, const void* data, size_t size, size_t offset) {
    if (!buffer || !data) {
        return;
    }

    if (offset + size > buffer->size) {
        return;
    }

    buffer->element_count =
        size / (buffer->type == BUFFER_TYPE_INDEX ? sizeof(u32) : buffer->layout.stride);
    if (buffer->backend->buffer.set_data) {
        buffer->backend->buffer.set_data(buffer, data, size, offset);
    }
}
