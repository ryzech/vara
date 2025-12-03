#pragma once

#include "vara/renderer/framebuffer.h"

typedef struct FramebufferBackendVT FramebufferBackendVT;

struct FramebufferBackendVT {
    b8 (*create)(Framebuffer* buffer, const FramebufferConfig* config);
    void (*destroy)(Framebuffer* buffer);
    void (*bind)(Framebuffer* buffer);
    void (*unbind)(Framebuffer* buffer);
    void (*resize)(Framebuffer* buffer, u32 width, u32 height);
};