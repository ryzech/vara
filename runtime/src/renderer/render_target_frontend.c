#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_target.h"
#include "vara/renderer/renderer.h"

RenderTarget* _render_target_create(RendererBackend* backend, const RenderTargetConfig* config) {
    RenderTarget* target = vara_allocate(sizeof(RenderTarget));
    vara_zero_memory(target, sizeof(RenderTarget));

    target->name = config->name;
    target->width = config->width;
    target->height = config->height;
    target->samples = config->samples;
    target->attachment_count = config->attachment_count;

    target->attachments = vara_allocate(sizeof(RenderTargetAttachment) * config->attachment_count);
    vara_zero_memory(
        target->attachments, sizeof(RenderTargetAttachment) * config->attachment_count
    );

    for (u32 i = 0; i < config->attachment_count; ++i) {
        const RenderTargetAttachmentConfig* src = &config->attachments[i];
        RenderTargetAttachment* dst = &target->attachments[i];

        dst->type = src->type;

        if (src->texture) {
            dst->texture = src->texture;
        }
    }

    target->backend = backend;

    if (target->backend->render_target.create) {
        if (!target->backend->render_target.create(target, config)) {
            render_target_destroy(target);
            return NULL;
        }
    }

    return target;
}

RenderTarget* render_target_create(Renderer* renderer, const RenderTargetConfig* config) {
    return _render_target_create(renderer->backend, config);
}

void render_target_destroy(RenderTarget* target) {
    if (target) {
        if (target->backend->render_target.destroy) {
            target->backend->render_target.destroy(target);
        }

        if (target->attachments) {
            vara_free(
                target->attachments, sizeof(RenderTargetAttachment) * target->attachment_count
            );
        }
        vara_free(target, sizeof(RenderTarget));
    }
}

void render_target_resize(RenderTarget* target, const u32 width, const u32 height) {
    if (target) {
        target->width = width;
        target->height = height;
        if (target->backend->render_target.resize) {
            target->backend->render_target.resize(target, width, height);
        }
    }
}

Texture* render_target_get_attachment(RenderTarget* target, const u32 index) {
    if (!target || index >= target->attachment_count) {
        return NULL;
    }

    return target->attachments[index].texture;
}
