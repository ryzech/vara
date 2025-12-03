#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/internal/renderer_internal.h"

Buffer* buffer_create(const BufferConfig* config) {
    Buffer* buffer = platform_allocate(sizeof(Buffer));
    platform_zero_memory(buffer, sizeof(Buffer));

    buffer->type = config->type;
    buffer->usage = config->usage;
    buffer->size = config->size;

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
                platform_allocate(sizeof(VertexAttribute) * config->layout->attribute_count);
            platform_copy_memory(
                buffer->layout.attributes,
                config->layout->attributes,
                sizeof(VertexAttribute) * config->layout->attribute_count
            );
        }
    }

    const RendererBackend* backend = renderer_backend_get();
    if (!backend->buffer.create(buffer, config)) {
        buffer_destroy(buffer);
        return NULL;
    }

    return buffer;
}

void buffer_destroy(Buffer* buffer) {
    if (buffer) {
        const RendererBackend* backend = renderer_backend_get();
        backend->buffer.destroy(buffer);

        if (buffer->layout.attributes) {
            platform_free(buffer->layout.attributes);
        }
        platform_free(buffer);
    }
}

void buffer_bind(Buffer* buffer) {
    if (buffer) {
        const RendererBackend* backend = renderer_backend_get();
        backend->buffer.bind(buffer);
    }
}

void buffer_unbind(Buffer* buffer) {
    if (buffer) {
        const RendererBackend* backend = renderer_backend_get();
        backend->buffer.unbind(buffer);
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

    const RendererBackend* backend = renderer_backend_get();
    backend->buffer.set_data(buffer, data, size, offset);
}
