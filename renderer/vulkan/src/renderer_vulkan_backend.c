#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "volk/volk.h"

typedef struct VulkanRendererState {
    VaraWindow* window;
    VkInstance instance;
} VulkanRendererState;

static VulkanRendererState renderer_state;

static b8 renderer_vulkan_create(void) {
    volkInitialize();
    const VkApplicationInfo application = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_2,
        .pApplicationName = renderer_state.window->name,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Vara Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    const char* required_extensions[3] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };

    const VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application,
        .enabledExtensionCount = 2,
        .ppEnabledExtensionNames = required_extensions,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
    };

    const VkResult instance = vkCreateInstance(&instance_info, NULL, &renderer_state.instance);
    if (instance != VK_SUCCESS) {
        FATAL("Failed to create VkInstance! Code: %u", instance);
        return false;
    }
    volkLoadInstance(renderer_state.instance);
    const u32 major = VK_VERSION_MAJOR(volkGetInstanceVersion());
    const u32 minor = VK_VERSION_MINOR(volkGetInstanceVersion());
    const u32 patch = VK_VERSION_PATCH(volkGetInstanceVersion());
    DEBUG("Loaded Vulkan %u.%u.%u", major, minor, patch);

    return true;
}

static void renderer_vulkan_destroy(void) {
    if (renderer_state.instance) {
        vkDestroyInstance(renderer_state.instance, NULL);
        renderer_state.instance = VK_NULL_HANDLE;
    }
    volkFinalize();
}

void renderer_vulkan_init(RendererBackend* backend, VaraWindow* window) {
    renderer_state.window = window;

    backend->name = "Vulkan";
    backend->type = RENDERER_TYPE_VULKAN;

    // Core Renderer
    backend->renderer.create = renderer_vulkan_create;
    backend->renderer.destroy = renderer_vulkan_destroy;

    DEBUG("Creating RendererBackend named('%s')", backend->name);
}
