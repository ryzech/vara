#include <GLFW/glfw3.h>
#include <vara/core/util/array.h>

#include "vara/renderer/vulkan_platform.h"

void vulkan_platform_get_required_extensions(const char*** array) {
    u32 extension_count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
    for (u32 i = 0; i < extension_count; i++) {
        array_append(*array, extensions[i]);
    }
}
