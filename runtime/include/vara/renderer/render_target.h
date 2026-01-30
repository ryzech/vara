#pragma once

#include <vara/core/defines.h>

// Forward declarations, in case they need access to each other.
typedef enum RenderTargetAttachmentType RenderTargetAttachmentType;
typedef struct RenderTargetAttachment RenderTargetAttachment;
typedef struct RenderTargetAttachmentConfig RenderTargetAttachmentConfig;
typedef struct RenderTarget RenderTarget;
typedef struct RenderTargetConfig RenderTargetConfig;

struct Texture;
struct Renderer;
struct RendererBackend;

enum RenderTargetAttachmentType {
    RENDER_TARGET_ATTACHMENT_COLOR,
    RENDER_TARGET_ATTACHMENT_DEPTH,
    RENDER_TARGET_ATTACHMENT_DEPTH_STENCIL
};

struct RenderTargetAttachmentConfig {
    RenderTargetAttachmentType type;
    struct Texture* texture;
};

struct RenderTargetAttachment {
    RenderTargetAttachmentType type;
    struct Texture* texture;
};

struct RenderTargetConfig {
    const char* name;
    u32 width, height;
    RenderTargetAttachmentConfig* attachments;
    u32 attachment_count;
    u16 samples;
};

struct RenderTarget {
    const char* name;
    u32 width, height;
    u16 samples;
    RenderTargetAttachment* attachments;
    u32 attachment_count;
    void* backend_data;
    struct RendererBackend* backend;
};

RenderTarget* render_target_create(struct Renderer* renderer, const RenderTargetConfig* config);
void render_target_destroy(RenderTarget* target);

void render_target_resize(RenderTarget* target, u32 width, u32 height);
struct Texture* render_target_get_attachment(RenderTarget* target, u32 index);
