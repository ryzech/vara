#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/types.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/core/util/string.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/swapchain_vulkan_backend.h"
#include "vara/renderer/vulkan_platform.h"
/* clang-format off */
#include <GLFW/glfw3.h>
/* clang-format on */

static b8 has_extension(VkExtensionProperties* available, const char* name) {
    for (u32 i = 0; i < array_length(available); i++) {
        if (strings_equal(name, available[i].extensionName)) {
            return true;
        }
    }
    return false;
}

static b8 renderer_vulkan_create(RendererBackend* backend) {
    VulkanRendererState* state = backend->backend_data;
    volkInitialize();
    const u32 major = VK_VERSION_MAJOR(volkGetInstanceVersion());
    const u32 minor = VK_VERSION_MINOR(volkGetInstanceVersion());
    const u32 patch = VK_VERSION_PATCH(volkGetInstanceVersion());
    const VkApplicationInfo application = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_MAKE_API_VERSION(0, major, minor, patch),
        .pApplicationName = state->window->name,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Vara Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    const char** required_extensions = array(const char*, NULL);
    const char** optional_extensions = array(const char*, NULL);
    vulkan_platform_get_required_extensions(&required_extensions);
#if defined(VARA_PLATFORM_APPLE)
    array_append(required_extensions, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
#if defined(VARA_DEBUG)
    array_append(optional_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    const char** enabled_extensions = array(const char*, NULL);

    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    VkExtensionProperties* available_extensions =
        array_sized(extension_count, VkExtensionProperties, NULL);
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, available_extensions);
    array_set_length(available_extensions, extension_count);

    DEBUG("Required Vulkan Extensions:")
    for (u32 i = 0; i < array_length(required_extensions); i++) {
        DEBUG("\t%s", required_extensions[i]);
    }

    for (u32 i = 0; i < array_length(required_extensions); i++) {
        if (!has_extension(available_extensions, required_extensions[i])) {
            FATAL("Missing required Vulkan extension: %s", required_extensions[i]);
            return false;
        }
        array_append(enabled_extensions, required_extensions[i]);
    }

    for (u32 i = 0; i < array_length(optional_extensions); i++) {
        if (!has_extension(available_extensions, optional_extensions[i])) {
            WARN("Missing optional Vulkan extension: %s", optional_extensions[i]);
            break;
        }
        array_append(enabled_extensions, optional_extensions[i]);
    }
    array_destroy(available_extensions);
    array_destroy(required_extensions);
    array_destroy(optional_extensions);

    const VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application,
        .enabledExtensionCount = array_length(enabled_extensions),
        .ppEnabledExtensionNames = enabled_extensions,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
#if defined(VARA_PLATFORM_APPLE)
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
    };

    const VkResult instance = vkCreateInstance(&instance_info, NULL, &state->instance);
    if (instance != VK_SUCCESS) {
        FATAL("Failed to create VkInstance! Code: %u", instance);
        return false;
    }
    array_destroy(enabled_extensions);
    volkLoadInstance(state->instance);

    const VkResult surface = glfwCreateWindowSurface(
        state->instance, platform_window_get_native_handle(state->window), NULL, &state->surface
    );
    if (surface != VK_SUCCESS) {
        FATAL("Failed to create VkSurfaceKHR! Code: %u", surface);
        return false;
    }

    if (!vulkan_device_create(state->instance, state->surface, &state->device)) {
        FATAL("Failed to create VulkanDevice!");
        return false;
    }

    return true;
}

static void renderer_vulkan_destroy(RendererBackend* backend) {
    VulkanRendererState* state = backend->backend_data;
    if (!state) {
        return;
    }

    if (state->surface) {
        vkDestroySurfaceKHR(state->instance, state->surface, NULL);
        state->surface = VK_NULL_HANDLE;
    }
    if (state->device.logical_device) {
        vkDestroyDevice(state->device.logical_device, NULL);
        state->device.logical_device = VK_NULL_HANDLE;
    }
    if (state->instance) {
        vkDestroyInstance(state->instance, NULL);
        state->instance = VK_NULL_HANDLE;
    }
    volkFinalize();
    vara_free(state, sizeof(VulkanRendererState));
}

void renderer_vulkan_init(RendererBackend* backend, VaraWindow* window) {
    VulkanRendererState* state = vara_allocate(sizeof(VulkanRendererState));
    if (!state) {
        return;
    }

    backend->backend_data = state;
    state->window = window;

    backend->name = "Vulkan";
    backend->type = RENDERER_TYPE_VULKAN;

    // Core Renderer
    backend->renderer.create = renderer_vulkan_create;
    backend->renderer.destroy = renderer_vulkan_destroy;

    // Swapchain
    backend->swapchain.create = swapchain_vulkan_create;
    backend->swapchain.destroy = swapchain_vulkan_destroy;
    backend->swapchain.present = swapchain_vulkan_present;

    DEBUG("Creating RendererBackend named('%s')", backend->name);
}
