#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

// Forward declarations, in case they need access to each other.
typedef struct RendererInstanceVT RendererInstanceVT;
typedef struct RendererInstance RendererInstance;

struct RenderCommandBuffer;

struct RendererInstanceVT {
    b8 (*renderer_create)(void);
    void (*renderer_destroy)(void);
    void (*renderer_clear)(void);
    void (*renderer_clear_color)(Vector4 color);
    void (*renderer_set_viewport)(Vector2i viewport_size);
    void (*renderer_execute_commands)(struct RenderCommandBuffer* buffer);
    void (*renderer_present)(void);
};

/**
 * RendererInstance holds pointers to the functions implemented
 * by each Renderer API. 
 *
 * This is so multiple renderers can be supported
 * and users can select (for example on Steam) a different one if there are 
 * any issues encountered.
 */
struct RendererInstance {
    RendererInstanceVT vt;
    const char* name;
    PlatformRendererType renderer_type;
};

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
 * Get RendererInstance singleton.
 * @return RendererInstance if exists, else NULL.
 */
RendererInstance* renderer_get_instance(void);

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
