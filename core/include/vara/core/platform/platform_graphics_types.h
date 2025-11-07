#pragma once

typedef enum PlatformGraphicsType {
    GRAPHICS_TYPE_NONE = 0,
    GRAPHICS_TYPE_OPENGL = 1,
    GRAPHICS_TYPE_VULKAN = 2,
    GRAPHICS_TYPE_DIRECTX = 3,
    GRAPHICS_TYPE_METAL = 4
} PlatformGraphicsType;
