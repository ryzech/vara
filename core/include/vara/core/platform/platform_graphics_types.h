#pragma once

typedef enum PlatformGraphicsType {
    GRAPHICS_TYPE_NONE = 0,
    GRAPHICS_TYPE_OPENGL = 1,
    GRAPHICS_TYPE_VULKAN = 2,
    GRAPHICS_TYPE_DIRECTX = 3,
    GRAPHICS_TYPE_METAL = 4
} PlatformGraphicsType;

static inline char* graphics_type_to_string(PlatformGraphicsType type) {
    switch (type) {
        case GRAPHICS_TYPE_NONE:
            return "None";
        case GRAPHICS_TYPE_OPENGL:
            return "OpenGL";
        case GRAPHICS_TYPE_VULKAN:
            return "Vulkan";
        case GRAPHICS_TYPE_DIRECTX:
            return "DirectX";
        case GRAPHICS_TYPE_METAL:
            return "Metal";
        default:
            return "Unknown";
    }
}
