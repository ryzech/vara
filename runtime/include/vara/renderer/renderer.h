#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/framebuffer.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/shader.h"
#include "vara/renderer/texture.h"

// Forward declarations, in case they need access to each other.
typedef struct RendererInstanceVT RendererInstanceVT;
typedef struct RendererInstance RendererInstance;

struct RendererInstanceVT {
    b8 (*renderer_create)(void);
    void (*renderer_destroy)(void);
    void (*renderer_clear)(void);
    void (*renderer_clear_color)(Vector4 color);
    void (*renderer_set_viewport)(Vector2i position, Vector2i size);
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
 * Get RendererInstance singleton.
 * @return RendererInstance if exists, else NULL.
 */
RendererInstance* renderer_get_instance(void);

/**
 * Get the active PlatformGraphicsType (i.e. OpenGL, Vulkan, etc.)
 * @return Type of active RendererInstance.
 */
PlatformRendererType renderer_get_renderer_type(void);

RenderCommandBuffer* renderer_get_frame_command_buffer(void);

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
 * Update the viewport for the given Renderer.
 * @param position Position of the viewport.
 * @param size Size of the viewport.
 */
// This will need moved out of here and likely into the framebuffer.
// For now in order to get this working, it will be here.
void renderer_set_viewport(Vector2i position, Vector2i size);

/**
 * This submits all the commands in the pass queue.\n
 * In the case of OpenGL this simply swaps the buffers.
 */
void renderer_present(void);
