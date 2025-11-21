#pragma once

#include <vara/core/defines.h>

// Forward declarations, in case they need access to each other.
typedef struct FramebufferVT FramebufferVT;
typedef struct Framebuffer Framebuffer;
typedef struct FramebufferConfig FramebufferConfig;

// Redeclaration's so we don't have to include header.
struct RendererInstance;

struct FramebufferConfig {
    const char* name;
    u32 width;
    u32 height;
};

struct Framebuffer {
    const char* name;
    struct RendererInstance* instance;
};
