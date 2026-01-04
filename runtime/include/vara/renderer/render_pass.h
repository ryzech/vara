#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

// Forward declarations, in case they need access to each other.
typedef struct RenderPass RenderPass;
typedef struct RenderPassConfig RenderPassConfig;
typedef enum AttachmentLoadOp AttachmentLoadOp;
typedef enum AttachmentStoreOp AttachmentStoreOp;
typedef struct RenderPassAttachment RenderPassAttachment;

// Redeclaration's so we don't have to include header.
struct Framebuffer;
struct RenderCommandBuffer;
struct Renderer;
struct RendererBackend;
struct RenderPacket;

enum AttachmentLoadOp {
    ATTACHMENT_LOAD_OP_LOAD,
    ATTACHMENT_LOAD_OP_CLEAR,
    ATTACHMENT_LOAD_OP_DONT_CARE,
};

enum AttachmentStoreOp {
    ATTACHMENT_STORE_OP_STORE,
    ATTACHMENT_STORE_OP_DONT_CARE,
};

struct RenderPassAttachment {
    AttachmentLoadOp load;
    AttachmentStoreOp store;
    Vector4 clear;
};

struct RenderPassConfig {
    const char* name;
    struct Framebuffer* target;
    RenderPassAttachment* color_attachments;
    u32 color_attachment_count;
    RenderPassAttachment* depth_stencil_attachment;
};

struct RenderPass {
    const char* name;
    struct Framebuffer* target;
    RenderPassAttachment* color_attachments;
    u32 color_attachment_count;
    RenderPassAttachment* depth_stencil_attachment;

    struct RenderPacket* packets;
    u32 packet_count;
    u32 packet_capacity;
    struct RenderCommandBuffer* command_buffer;
    struct RendererBackend* backend;
    void* backend_data;
};

RenderPass* render_pass_create(struct Renderer* renderer, const RenderPassConfig* config);
void render_pass_destroy(RenderPass* pass);

void render_pass_begin(RenderPass* pass);
void render_pass_end(struct Renderer* renderer, RenderPass* pass);
void render_pass_submit(RenderPass* pass, struct RenderPacket* packet);
