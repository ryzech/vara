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

    buffer->config = platform_allocate(sizeof(BufferConfig));
    platform_copy_memory(buffer->config, config, sizeof(BufferConfig));

    if (config->layout) {
        buffer->config->layout = platform_allocate(sizeof(VertexLayout));
        platform_copy_memory(
            buffer->config->layout, config->layout, sizeof(VertexLayout)
        );
        const u64 attribute_size =
            sizeof(VertexAttribute) * config->layout->attribute_count;
        buffer->config->layout->attributes = platform_allocate(attribute_size);
        platform_copy_memory(
            buffer->config->layout->attributes,
            config->layout->attributes,
            attribute_size
        );
    }

    // Calculate element count
    switch (buffer->config->type) {
        case BUFFER_TYPE_VERTEX:
            buffer->element_count =
                (i32)(config->size / buffer->config->layout->stride);
            break;
        case BUFFER_TYPE_INDEX:
            buffer->element_count = (i32)(config->size / sizeof(u32));
            break;
        default:
            buffer->element_count = 0;
            break;
    }

    const RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                buffer_opengl_init(buffer);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(instance->renderer_type)
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

        if (buffer->config->layout) {
            if (buffer->config->layout->attributes) {
                platform_free(buffer->config->layout->attributes);
            }
            platform_free(buffer->config->layout);
        }
        platform_free(buffer->config);
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
