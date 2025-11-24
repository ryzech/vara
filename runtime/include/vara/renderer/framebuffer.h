#pragma once

#include <vara/core/defines.h>

// Forward declarations, in case they need access to each other.
typedef enum FramebufferAttachmentType FramebufferAttachmentType;
typedef enum FramebufferAttachmentFormat FramebufferAttachmentFormat;
typedef struct FramebufferAttachmentConfig FramebufferAttachmentConfig;
typedef struct FramebufferVT FramebufferVT;
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
    u32 width;
    u32 height;
    FramebufferAttachmentConfig* attachments;
    size_t attachment_count;
    u16 samples;
};

struct FramebufferVT {
    b8 (*framebuffer_create)(
        Framebuffer* buffer, const FramebufferConfig* config
    );
    void (*framebuffer_destroy)(Framebuffer* buffer);
    void (*framebuffer_bind)(Framebuffer* buffer);
    void (*framebuffer_unbind)(Framebuffer* buffer);
    void (*framebuffer_resize)(Framebuffer* buffer, u32 width, u32 height);
};

struct Framebuffer {
    const char* name;
    FramebufferConfig* config;
    FramebufferVT vt;
    u32 width;
    u32 height;
    u16 samples;
    void* backend_data;
};

Framebuffer* framebuffer_create(const FramebufferConfig* config);
void framebuffer_destroy(Framebuffer* buffer);

void framebuffer_bind(Framebuffer* buffer);
void framebuffer_unbind(Framebuffer* buffer);
void framebuffer_resize(Framebuffer* buffer, u32 width, u32 height);
