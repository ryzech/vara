#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/renderer.h"

extern void buffer_opengl_init(Buffer* buffer);

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

        if (config->layout->attributes > 0) {
            buffer->layout.attributes = platform_allocate(
                sizeof(VertexAttribute) * config->layout->attribute_count
            );
            platform_copy_memory(
                buffer->layout.attributes,
                config->layout->attributes,
                sizeof(VertexAttribute) * config->layout->attribute_count
            );
        }
    }

    const RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case RENDERER_TYPE_OPENGL:
                buffer_opengl_init(buffer);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    renderer_type_to_string(instance->renderer_type)
                );
                return NULL;
        }
    }

    if (!buffer->vt.buffer_create(buffer, config)) {
        buffer_destroy(buffer);
        return NULL;
    }

    return buffer;
}

void buffer_destroy(Buffer* buffer) {
    if (buffer && buffer->vt.buffer_destroy) {
        buffer->vt.buffer_destroy(buffer);

        if (buffer->layout.attributes) {
            platform_free(buffer->layout.attributes);
        }
        platform_free(buffer);
    }
}

void buffer_bind(Buffer* buffer) {
    if (buffer && buffer->vt.buffer_bind) {
        buffer->vt.buffer_bind(buffer);
    }
}

void buffer_unbind(Buffer* buffer) {
    if (buffer && buffer->vt.buffer_unbind) {
        buffer->vt.buffer_unbind(buffer);
    }
}
