#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/framebuffer.h"
#include "vara/renderer/renderer.h"

extern void framebuffer_opengl_init(Framebuffer* buffer);

Framebuffer* framebuffer_create(const FramebufferConfig* config) {
    Framebuffer* buffer = platform_allocate(sizeof(Framebuffer));
    platform_zero_memory(buffer, sizeof(Framebuffer));

    // Yes I know I should just have the types in the FB struct.
    // I will clean this up later so we don't need code duplication.
    buffer->config = platform_allocate(sizeof(FramebufferConfig));
    platform_copy_memory(buffer->config, config, sizeof(FramebufferConfig));

    buffer->config->attachments = platform_allocate(
        sizeof(FramebufferAttachmentConfig) * config->attachment_count
    );
    platform_copy_memory(
        buffer->config->attachments,
        config->attachments,
        sizeof(FramebufferAttachmentConfig) * config->attachment_count
    );

    // Maybe should just rely on the config
    // And remove these from the main struct?
    buffer->name = config->name;
    buffer->width = config->width;
    buffer->height = config->height;
    buffer->samples = config->samples;

    const RendererInstance* instance = renderer_get_instance();
    if (instance) {
        switch (instance->renderer_type) {
            case GRAPHICS_TYPE_OPENGL:
                framebuffer_opengl_init(buffer);
                break;
            default:
                ERROR(
                    "Unsupported graphics type: %s",
                    graphics_type_to_string(instance->renderer_type)
                );
                return NULL;
        }
    }

    if (!buffer->vt.framebuffer_create(buffer, config)) {
        framebuffer_destroy(buffer);
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

void framebuffer_resize(
    Framebuffer* buffer, const u32 width, const u32 height
) {
    if (buffer && buffer->vt.framebuffer_resize) {
        buffer->vt.framebuffer_resize(buffer, width, height);
    }
}
