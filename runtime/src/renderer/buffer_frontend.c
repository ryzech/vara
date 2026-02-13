#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/renderer.h"

Buffer* _buffer_create(RendererBackend* backend, const BufferConfig* config) {
    Buffer* buffer = vara_allocate(sizeof(Buffer));
    vara_zero_memory(buffer, sizeof(Buffer));

    buffer->type = config->type;
    buffer->usage = config->usage;
    buffer->size = config->size;
    buffer->binding = config->binding;
    buffer->backend = backend;

    if (buffer->backend->buffer.create) {
        if (!buffer->backend->buffer.create(buffer, config)) {
            buffer_destroy(buffer);
            return NULL;
        }
    }

    return buffer;
}

Buffer* buffer_create(Renderer* renderer, const BufferConfig* config) {
    return _buffer_create(renderer->backend, config);
}

void buffer_destroy(Buffer* buffer) {
    if (buffer) {
        if (buffer->backend->buffer.destroy) {
            buffer->backend->buffer.destroy(buffer);
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

    if (buffer->backend->buffer.set_data) {
        buffer->backend->buffer.set_data(buffer, data, size, offset);
    }
}
