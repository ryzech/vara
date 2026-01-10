#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/framebuffer.h"
#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/renderer.h"

Framebuffer* framebuffer_create(Renderer* renderer, const FramebufferConfig* config) {
    Framebuffer* buffer = vara_allocate(sizeof(Framebuffer));
    vara_zero_memory(buffer, sizeof(Framebuffer));

    buffer->attachments =
        vara_allocate(sizeof(FramebufferAttachmentConfig) * config->attachment_count);
    vara_copy_memory(
        buffer->attachments,
        config->attachments,
        sizeof(FramebufferAttachmentConfig) * config->attachment_count
    );

    buffer->name = config->name;
    buffer->width = config->width;
    buffer->height = config->height;
    buffer->samples = config->samples;
    buffer->attachment_count = config->attachment_count;

    RendererBackend* backend = renderer_backend_get(renderer);
    buffer->backend = backend;

    if (!buffer->backend->framebuffer.create(buffer, config)) {
        framebuffer_destroy(buffer);
        return NULL;
    }

    return buffer;
}

void framebuffer_destroy(Framebuffer* buffer) {
    if (buffer) {
        buffer->backend->framebuffer.destroy(buffer);

        if (buffer->attachments) {
            vara_free(
                buffer->attachments, sizeof(FramebufferAttachmentConfig) * buffer->attachment_count
            );
        }
        vara_free(buffer, sizeof(Buffer));
    }
}

void framebuffer_bind(Framebuffer* buffer) {
    if (buffer) {
        buffer->backend->framebuffer.bind(buffer);
    }
}

void framebuffer_unbind(Framebuffer* buffer) {
    if (buffer) {
        buffer->backend->framebuffer.unbind(buffer);
    }
}

void framebuffer_resize(Framebuffer* buffer, const u32 width, const u32 height) {
    if (buffer) {
        buffer->backend->framebuffer.resize(buffer, width, height);
    }
}

Texture* framebuffer_get_attachment(Framebuffer* buffer, const u32 index) {
    if (!buffer || !buffer->attachment_textures || index >= buffer->attachment_count) {
        return NULL;
    }

    return buffer->attachment_textures[index];
}
