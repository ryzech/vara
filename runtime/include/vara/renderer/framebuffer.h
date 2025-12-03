#pragma once

#include <vara/core/defines.h>

// Forward declarations, in case they need access to each other.
typedef enum FramebufferAttachmentType FramebufferAttachmentType;
typedef enum FramebufferAttachmentFormat FramebufferAttachmentFormat;
typedef struct FramebufferAttachmentConfig FramebufferAttachmentConfig;
typedef struct Framebuffer Framebuffer;
typedef struct FramebufferConfig FramebufferConfig;

enum FramebufferAttachmentType {
    FRAMEBUFFER_ATTACHMENT_COLOR,
    FRAMEBUFFER_ATTACHMENT_DEPTH,
    FRAMEBUFFER_ATTACHMENT_STENCIL,
    FRAMEBUFFER_ATTACHMENT_DEPTH_STENCIL
};

enum FramebufferAttachmentFormat {
    FRAMEBUFFER_FORMAT_RGBA8,

    FRAMEBUFFER_FORMAT_DEPTH24_STENCIL8,
};

struct FramebufferAttachmentConfig {
    FramebufferAttachmentType type;
    FramebufferAttachmentFormat format;
};

struct FramebufferConfig {
    const char* name;
    FramebufferAttachmentConfig* attachments;
    u32 width, height;
    u32 attachment_count;
    u16 samples;
};

struct Framebuffer {
    const char* name;
    FramebufferAttachmentConfig* attachments;
    void* backend_data;
    u32 width, height;
    u32 attachment_count;
    u16 samples;
};

Framebuffer* framebuffer_create(const FramebufferConfig* config);
void framebuffer_destroy(Framebuffer* buffer);

void framebuffer_bind(Framebuffer* buffer);
void framebuffer_unbind(Framebuffer* buffer);
void framebuffer_resize(Framebuffer* buffer, u32 width, u32 height);
