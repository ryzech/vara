#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

/**
 * Creates a new RendererInstance singleton and selects specified backend.
 * @param window VaraWindow to create the RendererInstance in.
 * @return True if the Renderer was created, else False
 */
b8 renderer_create(VaraWindow* window);

/**
 * Cleanup associated resources with the RendererInstance.
 */
void renderer_destroy(void);

/**
 * Window resize callback.
 * @param new_size New window size.
 */
void renderer_on_window_resize(Vector2i new_size);

/**
 * Get the active PlatformGraphicsType (i.e. OpenGL, Vulkan, etc.)
 * @return Type of active RendererInstance.
 */
PlatformRendererType renderer_get_renderer_type(void);

struct RenderCommandBuffer* renderer_get_frame_command_buffer(void);

void renderer_begin_frame(void);

void renderer_end_frame(void);

/**
 * Clear the screen from the previous frame.\n
 * Clears both color and depth buffers.
 */
void renderer_clear(void);

/**
 * Sets the color for the next screen clear.
 * @param color Color to use when clearing the screen.
 */
void renderer_clear_color(Vector4 color);

/**
 * Execute commands in given RenderCommandBuffer.
 * @param buffer RenderCommandBuffer to execute commands of.
 */
void renderer_execute_commands(struct RenderCommandBuffer* buffer);

/**
 * This submits all the commands in the pass queue.\n
 * In the case of OpenGL this simply swaps the buffers.
 */
void renderer_present(void);
