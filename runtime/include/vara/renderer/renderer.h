#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

typedef struct Renderer Renderer;

struct RendererBackend;
struct RenderCommandBuffer;

struct Renderer {
    struct RendererBackend* backend;
    struct RenderCommandBuffer* command_buffer;
    VaraWindow* window;
};

/**
 * Creates a new RendererInstance singleton and selects specified backend.
 * @param window VaraWindow to create the RendererInstance in.
 * @return True if the Renderer was created, else False
 */
Renderer* renderer_create(VaraWindow* window);

/**
 * Cleanup associated resources with the RendererInstance.
 */
void renderer_destroy(Renderer* renderer);

/**
 * Window resize callback.
 * @param new_size New window size.
 */
void renderer_on_window_resize(Renderer* renderer, Vector2i new_size);

/**
 * Get the active PlatformGraphicsType (i.e. OpenGL, Vulkan, etc.)
 * @return Type of active RendererInstance.
 */
PlatformRendererType renderer_get_renderer_type(Renderer* renderer);

struct RenderCommandBuffer* renderer_get_frame_command_buffer(Renderer* renderer);

void renderer_begin_frame(Renderer* renderer);

void renderer_end_frame(Renderer* renderer);

/**
 * Clear the screen from the previous frame.\n
 * Clears both color and depth buffers.
 */
void renderer_clear(Renderer* renderer);

/**
 * Sets the color for the next screen clear.
 * @param color Color to use when clearing the screen.
 */
void renderer_clear_color(Renderer* renderer, Vector4 color);

/**
 * Execute commands in given RenderCommandBuffer.
 * @param buffer RenderCommandBuffer to execute commands of.
 */
void renderer_execute_commands(Renderer* renderer, struct RenderCommandBuffer* buffer);

/**
 * This submits all the commands in the pass queue.\n
 * In the case of OpenGL this simply swaps the buffers.
 */
void renderer_present(Renderer* renderer);
