#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/framebuffer.h"
#include "vara/renderer/renderer.h"

extern Framebuffer* framebuffer_opengl_init(const FramebufferConfig* config);

Framebuffer* framebuffer_create(const FramebufferConfig* config) {
    Framebuffer* buffer = NULL;
    RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                buffer = framebuffer_opengl_init(config);
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

    buffer->name = config->name;
    buffer->width = config->width;
    buffer->height = config->height;
    buffer->samples = config->samples;

    if (!buffer->vt.framebuffer_create(buffer, config)) {
        platform_free(buffer->config->attachments);
        platform_free(buffer->config);
        platform_free(buffer);
        return NULL;
    }

    return buffer;
}

void framebuffer_destroy(Framebuffer* buffer) {
    if (buffer && buffer->vt.framebuffer_destroy) {
        buffer->vt.framebuffer_destroy(buffer);

        if (buffer->config) {
            if (buffer->config->attachments) {
                platform_free(buffer->config->attachments);
            }
            platform_free(buffer->config);
        }

        platform_free(buffer);
    }
}

void framebuffer_bind(Framebuffer* buffer) {
    if (buffer && buffer->vt.framebuffer_bind) {
        buffer->vt.framebuffer_bind(buffer);
    }
}

void framebuffer_unbind(Framebuffer* buffer) {
    if (buffer && buffer->vt.framebuffer_unbind) {
        buffer->vt.framebuffer_unbind(buffer);
    }
}

void framebuffer_resize(Framebuffer* buffer, u32 width, u32 height) {
    if (buffer && buffer->vt.framebuffer_resize) {
        buffer->vt.framebuffer_resize(buffer, width, height);
    }
}
