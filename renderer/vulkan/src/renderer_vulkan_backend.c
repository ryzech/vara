#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/core/util/string.h"
#include "volk/volk.h"

typedef struct VulkanRendererState {
    VaraWindow* window;
    VkInstance instance;
    const char** required_extensions;
    const char** enabled_extensions;
} VulkanRendererState;

static VulkanRendererState renderer_state;

static b8 has_extension(VkExtensionProperties* available, const char* name) {
    for (u32 i = 0; i < array_length(available); i++) {
        if (strings_equal(name, available[i].extensionName)) {
            return true;
        }
    }
    return false;
}

static b8 renderer_vulkan_create(void) {
    volkInitialize();
    const u32 major = VK_VERSION_MAJOR(volkGetInstanceVersion());
    const u32 minor = VK_VERSION_MINOR(volkGetInstanceVersion());
    const u32 patch = VK_VERSION_PATCH(volkGetInstanceVersion());
    const VkApplicationInfo application = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_MAKE_API_VERSION(0, major, minor, patch),
        .pApplicationName = renderer_state.window->name,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Vara Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    renderer_state.required_extensions = array(const char*, NULL);
    array_append(renderer_state.required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VARA_PLATFORM_APPLE)
    array_append(
        renderer_state.required_extensions, &VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    );
#endif
#if defined(VARA_DEBUG)
    array_append(renderer_state.required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    renderer_state.enabled_extensions = array(const char*, NULL);

    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    VkExtensionProperties* available_extensions =
        array_sized(extension_count, VkExtensionProperties, NULL);
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, available_extensions);
    array_set_length(available_extensions, extension_count);

    DEBUG("Available Vulkan Extensions:")
    for (u32 i = 0; i < array_length(available_extensions); i++) {
        DEBUG("\t%s", available_extensions[i].extensionName);
    }

    for (u32 i = 0; i < array_length(renderer_state.required_extensions); i++) {
        if (!has_extension(available_extensions, renderer_state.required_extensions[i])) {
            FATAL("Missing required Vulkan extension: %s", renderer_state.required_extensions[i]);
            return false;
        }
        array_append(renderer_state.enabled_extensions, renderer_state.required_extensions[i]);
    }
    array_destroy(available_extensions);

    const VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application,
        .enabledExtensionCount = array_length(renderer_state.enabled_extensions),
        .ppEnabledExtensionNames = renderer_state.enabled_extensions,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
#if defined(VARA_PLATFORM_APPLE)
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
    };

    const VkResult instance = vkCreateInstance(&instance_info, NULL, &renderer_state.instance);
    if (instance != VK_SUCCESS) {
        FATAL("Failed to create VkInstance! Code: %u", instance);
        return false;
    }
    volkLoadInstance(renderer_state.instance);
    DEBUG("Loaded Vulkan %u.%u.%u", major, minor, patch);

    return true;
}

static void renderer_vulkan_destroy(void) {
    if (renderer_state.instance) {
        vkDestroyInstance(renderer_state.instance, NULL);
        renderer_state.instance = VK_NULL_HANDLE;
    }
    array_destroy(renderer_state.required_extensions);
    array_destroy(renderer_state.enabled_extensions);
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
