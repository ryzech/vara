#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/types.h>
#include <vara/core/memory/memory.h>
#include <vara/core/platform/platform_window.h>
#include <vara/core/util/array.h>
#include <vara/core/util/string.h>
#include <vara/renderer/internal/renderer_internal.h>

#include "vara/renderer/buffer_vulkan_backend.h"
#include "vara/renderer/render_pass_vulkan_backend.h"
#include "vara/renderer/render_pipeline_vulkan_backend.h"
#include "vara/renderer/renderer_vulkan_backend.h"
#include "vara/renderer/shader_vulkan_backend.h"
#include "vara/renderer/swapchain_vulkan_backend.h"
#include "vara/renderer/vulkan_platform.h"
#include "vara/renderer/vulkan_utils.h"

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
    // Add allocator callbacks later.
    state->allocator = NULL;

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
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
    VkExtensionProperties* available_extensions =
        array_sized(extension_count, VkExtensionProperties, NULL);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, available_extensions));
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

    const char* validation_layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };

    const VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application,
        .enabledExtensionCount = array_length(enabled_extensions),
        .ppEnabledExtensionNames = enabled_extensions,
#if defined(VARA_DEBUG)
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = validation_layers,
#else
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
#endif
#if defined(VARA_PLATFORM_APPLE)
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
    };

    const VkResult instance = vkCreateInstance(&instance_info, state->allocator, &state->instance);
    if (instance != VK_SUCCESS) {
        FATAL("Failed to create VkInstance! Code: %u", instance);
        return false;
    }
    array_destroy(enabled_extensions);
    volkLoadInstance(state->instance);

    const VkResult surface = glfwCreateWindowSurface(
        state->instance,
        platform_window_get_native_handle(state->window),
        state->allocator,
        &state->surface
    );
    if (surface != VK_SUCCESS) {
        FATAL("Failed to create VkSurfaceKHR! Code: %u", surface);
        return false;
    }

    if (!vulkan_device_create(state->instance, state->surface, &state->device)) {
        FATAL("Failed to create VulkanDevice!");
        return false;
    }

    VmaAllocatorCreateInfo vma_info = {
        .physicalDevice = state->device.physical_device,
        .device = state->device.logical_device,
        .instance = state->instance,
        .vulkanApiVersion = VK_MAKE_API_VERSION(0, major, minor, patch),
        .pAllocationCallbacks = state->allocator,
    };

    VmaVulkanFunctions vulkan_functions;
    VkResult result = vmaImportVulkanFunctionsFromVolk(&vma_info, &vulkan_functions);
    if (result != VK_SUCCESS) {
        FATAL("Failed to retrieve vulkan functions from Volk.");
        return false;
    }
    vma_info.pVulkanFunctions = &vulkan_functions;

    result = vmaCreateAllocator(&vma_info, &state->vma_allocator);
    if (result != VK_SUCCESS) {
        FATAL("Failed to create VmaAllocator.");
        return false;
    }

    return true;
}

static void renderer_vulkan_destroy(RendererBackend* backend) {
    VulkanRendererState* state = backend->backend_data;
    if (!state) {
        return;
    }

    vmaDestroyAllocator(state->vma_allocator);
    vulkan_device_destroy(&state->device);
    vkDestroySurfaceKHR(state->instance, state->surface, state->allocator);
    vkDestroyInstance(state->instance, state->allocator);
    volkFinalize();
    vara_free(state, sizeof(VulkanRendererState));
}

static void renderer_vulkan_submit(RendererBackend* backend, const RenderCommandBuffer* buffer) {
    VulkanRendererState* state = backend->backend_data;
    VulkanSwapchainState* swapchain = state->swapchain->backend_data;
    VulkanFrame* frame = &swapchain->frames[swapchain->current_frame];

    u8* cmd = buffer->buffer;
    const u8* end = buffer->buffer + buffer->used;

    while (cmd < end) {
        const RenderCommandHeader* header = (RenderCommandHeader*)cmd;

        switch (header->type) {
            case RENDER_CMD_BEGIN_PASS: {
                const RenderCmdBeginPass* begin_pass = (RenderCmdBeginPass*)cmd;
                render_pass_vulkan_begin(begin_pass->pass);
                break;
            }
            case RENDER_CMD_END_PASS: {
                const RenderCmdEndPass* end_pass = (RenderCmdEndPass*)cmd;
                render_pass_vulkan_end(end_pass->pass);
                break;
            }
            case RENDER_CMD_BIND_PIPELINE: {
                const RenderCmdBindPipeline* bind_pipeline = (RenderCmdBindPipeline*)cmd;
                render_pipeline_vulkan_bind(bind_pipeline->pipeline);
                break;
            }
            case RENDER_CMD_BIND_BUFFER: {
                const RenderCmdBindBuffer* bind_buffer = (RenderCmdBindBuffer*)cmd;
                VulkanBufferState* buffer_state = bind_buffer->buffer->backend_data;

                switch (bind_buffer->buffer->type) {
                    case BUFFER_TYPE_VERTEX: {
                        VkDeviceSize offset = 0;
                        vkCmdBindVertexBuffers(
                            frame->command_buffer, 0, 1, &buffer_state->buffer, &offset
                        );
                        break;
                    }
                    case BUFFER_TYPE_INDEX: {
                        vkCmdBindIndexBuffer(
                            frame->command_buffer, buffer_state->buffer, 0, VK_INDEX_TYPE_UINT32
                        );
                        break;
                    }
                    case BUFFER_TYPE_UNIFORM:
                        break;
                }

                break;
            }
            case RENDER_CMD_SET_VIEWPORT: {
                const RenderCmdSetViewport* viewport = (RenderCmdSetViewport*)cmd;
                VkViewport viewport_info = {
                    .x = 0,
                    .y = 0,
                    .width = viewport->width,
                    .height = viewport->height,
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f,
                };
                VkRect2D scissor_info = {
                    .offset = {0, 0},
                    .extent = swapchain->extent,
                };
                vkCmdSetViewport(frame->command_buffer, 0, 1, &viewport_info);
                vkCmdSetScissor(frame->command_buffer, 0, 1, &scissor_info);
                break;
            }
            case RENDER_CMD_DRAW_INDEXED: {
                const RenderCmdDrawIndexed* draw_indexed = (RenderCmdDrawIndexed*)cmd;
                vkCmdDrawIndexed(
                    frame->command_buffer,
                    draw_indexed->index_count,
                    1,
                    draw_indexed->first_index,
                    0,
                    0
                );
                break;
            }
            case RENDER_CMD_DRAW: {
                const RenderCmdDraw* draw = (RenderCmdDraw*)cmd;
                vkCmdDraw(frame->command_buffer, draw->vertex_count, 1, draw->first_vertex, 0);
                break;
            }
            default: {
                break;
            }
        }

        cmd += header->size;
    }
}

static void renderer_vulkan_begin_frame(RendererBackend* backend) {
    VulkanRendererState* state = backend->backend_data;
    VulkanSwapchainState* swapchain = state->swapchain->backend_data;
    VulkanFrame* frame = &swapchain->frames[swapchain->current_frame];

    swapchain_vulkan_begin_frame(state->swapchain);

    VK_CHECK(vkResetCommandPool(state->device.logical_device, frame->command_pool, 0));

    const VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VK_CHECK(vkBeginCommandBuffer(frame->command_buffer, &begin_info));
}

static void renderer_vulkan_end_frame(RendererBackend* backend) {
    VulkanRendererState* state = backend->backend_data;
    VulkanSwapchainState* swapchain = state->swapchain->backend_data;
    VulkanFrame* frame = &swapchain->frames[swapchain->current_frame];

    VK_CHECK(vkEndCommandBuffer(frame->command_buffer));

    VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &frame->image_available,
        .pWaitDstStageMask = &flags,
        .commandBufferCount = 1,
        .pCommandBuffers = &frame->command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &frame->render_finished,
    };
    VK_CHECK(vkQueueSubmit(state->device.graphics_queue, 1, &submit_info, frame->in_flight));
}

void renderer_vulkan_init(RendererBackend* backend, VaraWindow* window) {
    VulkanRendererState* state = vara_allocate(sizeof(VulkanRendererState));
    vara_zero_memory(state, sizeof(VulkanRendererState));
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
    backend->renderer.submit = renderer_vulkan_submit;
    backend->renderer.begin_frame = renderer_vulkan_begin_frame;
    backend->renderer.end_frame = renderer_vulkan_end_frame;

    // Swapchain
    backend->swapchain.create = swapchain_vulkan_create;
    backend->swapchain.destroy = swapchain_vulkan_destroy;
    backend->swapchain.present = swapchain_vulkan_present;

    // Render Pass
    backend->render_pass.create = render_pass_vulkan_create;
    backend->render_pass.destroy = render_pass_vulkan_destroy;
    backend->render_pass.begin = render_pass_vulkan_begin;
    backend->render_pass.end = render_pass_vulkan_end;

    // Shader
    backend->shader.create = shader_vulkan_create;
    backend->shader.destroy = shader_vulkan_destroy;
    backend->shader.set_mat4 = shader_vulkan_set_mat4;
    backend->shader.set_int_array = shader_vulkan_set_int_array;
    backend->shader.bind = shader_vulkan_bind;
    backend->shader.unbind = shader_vulkan_unbind;
    backend->shader.dispatch = shader_vulkan_dispatch;

    // Pipeline
    backend->render_pipeline.create = render_pipeline_vulkan_create;
    backend->render_pipeline.destroy = render_pipeline_vulkan_destroy;
    backend->render_pipeline.bind = render_pipeline_vulkan_bind;

    // Buffer
    backend->buffer.create = buffer_vulkan_create;
    backend->buffer.destroy = buffer_vulkan_destroy;
    backend->buffer.set_data = buffer_vulkan_set_data;

    DEBUG("Creating RendererBackend named('%s')", backend->name);
}
