#pragma once

typedef enum PlatformRendererType PlatformRendererType;

enum PlatformRendererType {
    RENDERER_TYPE_NONE = 0,
    RENDERER_TYPE_OPENGL = 1,
    RENDERER_TYPE_VULKAN = 2,
    RENDERER_TYPE_DIRECTX = 3,
    RENDERER_TYPE_METAL = 4
};

static char* renderer_type_to_string(PlatformRendererType type) {
    switch (type) {
        case RENDERER_TYPE_NONE:
            return "None";
        case RENDERER_TYPE_OPENGL:
            return "OpenGL";
        case RENDERER_TYPE_VULKAN:
            return "Vulkan";
        case RENDERER_TYPE_DIRECTX:
            return "DirectX";
        case RENDERER_TYPE_METAL:
            return "Metal";
        default:
            return "Unknown";
    }
}
