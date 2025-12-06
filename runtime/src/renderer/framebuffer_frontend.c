#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/framebuffer.h"
#include "vara/renderer/internal/renderer_internal.h"

Framebuffer* framebuffer_create(const FramebufferConfig* config) {
    Framebuffer* buffer = platform_allocate(sizeof(Framebuffer));
    platform_zero_memory(buffer, sizeof(Framebuffer));

    buffer->attachments =
        platform_allocate(sizeof(FramebufferAttachmentConfig) * config->attachment_count);
    platform_copy_memory(
        buffer->attachments,
        config->attachments,
        sizeof(FramebufferAttachmentConfig) * config->attachment_count
    );

    buffer->name = config->name;
    buffer->width = config->width;
    buffer->height = config->height;
    buffer->samples = config->samples;
    buffer->attachment_count = config->attachment_count;

    const RendererBackend* backend = renderer_backend_get();
    if (!backend->framebuffer.create(buffer, config)) {
        framebuffer_destroy(buffer);
        return NULL;
    }

    return buffer;
}

void framebuffer_destroy(Framebuffer* buffer) {
    if (buffer) {
        const RendererBackend* backend = renderer_backend_get();
        backend->framebuffer.destroy(buffer);

        if (buffer->attachments) {
            platform_free(buffer->attachments);
        }
        platform_free(buffer);
    }
}

void framebuffer_bind(Framebuffer* buffer) {
    if (buffer) {
        const RendererBackend* backend = renderer_backend_get();
        backend->framebuffer.bind(buffer);
    }
}

void framebuffer_unbind(Framebuffer* buffer) {
    if (buffer) {
        const RendererBackend* backend = renderer_backend_get();
        backend->framebuffer.unbind(buffer);
    }
}

void framebuffer_resize(Framebuffer* buffer, const u32 width, const u32 height) {
    if (buffer) {
        const RendererBackend* backend = renderer_backend_get();
        backend->framebuffer.resize(buffer, width, height);
    }
}

Texture* framebuffer_get_attachment(Framebuffer* buffer, const u32 index) {
    if (!buffer || !buffer->attachment_textures || index >= buffer->attachment_count) {
        return NULL;
    }

    return buffer->attachment_textures[index];
}
