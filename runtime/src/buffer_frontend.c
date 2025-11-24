#include <stdlib.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/renderer.h"

extern Buffer* buffer_opengl_init(const BufferConfig* config);

Buffer* buffer_create(const BufferConfig* config) {
    Buffer* buffer = NULL;
    RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                buffer = buffer_opengl_init(config);
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

    // Calculate element count
    // Can stay in here, since there's no allocation happening.
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

    if (!buffer->vt.buffer_create(buffer, config)) {
        if (buffer->config->layout) {
            if (buffer->config->layout->attributes) {
                platform_free(buffer->config->layout->attributes);
            }
            platform_free(buffer->config->layout);
        }
        platform_free(buffer->config);
        platform_free(buffer);
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
