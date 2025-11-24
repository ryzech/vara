#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/renderer.h"

extern Buffer* buffer_opengl_init(void);

Buffer* buffer_create(const BufferConfig* config) {
    Buffer* buffer = NULL;
    RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                buffer = buffer_opengl_init();
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(instance->renderer_type)
                );
                return NULL;
        }
    }

    if (!buffer) {
        return NULL;
    }

    buffer->type = config->type;
    buffer->usage = config->usage;
    buffer->size = config->size;

    // Probably should move this to the backend?
    if (config->layout) {
        buffer->layout = platform_allocate(sizeof(VertexLayout));
        buffer->layout->attribute_count = config->layout->attribute_count;
        buffer->layout->stride = config->layout->stride;
        u64 attribute_size =
            sizeof(VertexAttribute) * config->layout->attribute_count;
        buffer->layout->attributes = platform_allocate(attribute_size);
        platform_copy_memory(
            buffer->layout->attributes,
            config->layout->attributes,
            attribute_size
        );
    }

    switch (buffer->type) {
        case BUFFER_TYPE_VERTEX:
            buffer->element_count =
                (i32)(config->size / buffer->layout->stride);
            break;
        case BUFFER_TYPE_INDEX:
            buffer->element_count = (i32)(config->size / sizeof(u32));
            break;
        default:
            buffer->element_count = 0;
            break;
    }

    if (!buffer->vt.buffer_create(buffer, config)) {
        platform_free(buffer);
        if (buffer->layout) {
            if (buffer->layout->attributes) {
                platform_free(buffer->layout->attributes);
            }
            platform_free(buffer->layout);
        }
        return NULL;
    }

    return buffer;
}

void buffer_destroy(Buffer* buffer) {
    if (buffer && buffer->vt.buffer_destroy) {
        buffer->vt.buffer_destroy(buffer);

        if (buffer->layout) {
            if (buffer->layout->attributes) {
                platform_free(buffer->layout->attributes);
            }
            platform_free(buffer->layout);
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
