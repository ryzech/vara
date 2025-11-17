#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>

typedef struct RendererInstanceVT RendererInstanceVT;
typedef struct RendererInstance RendererInstance;

struct RendererInstanceVT {
    b8 (*renderer_create)(void);
    void (*renderer_destroy)(void);
    void (*renderer_clear)(void);
    void (*renderer_clear_color)(Vector4 color);
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
    const char* name;
    PlatformGraphicsType renderer_type;
    RendererInstanceVT vt;
};

/**
 * Creates a new RendererInstance and selects specified backend.
 * @param window VaraWindow to create the RendererInstance in.
 * @return A new RendererInstance in the selected VaraWindow.
 */
RendererInstance* renderer_create(VaraWindow* window);

/**
 * Cleanup associated resources with the RendererInstance.
 * @param instance RendererInstance to destroy.
 */
void renderer_destroy(RendererInstance* instance);

/**
 * Clear the screen from the previous frame.\n
 * Clears both color and depth buffers.
 * @param instance RendererInstance to clear the current screen.
 */
void renderer_clear(RendererInstance* instance);

/**
 * Sets the color for the next screen clear.
 * @param instance RendererInstance to set the clear color for.
 * @param color Color to use when clearing the screen.
 */
void renderer_clear_color(RendererInstance* instance, Vector4 color);

/**
 * This submits all the commands in the pass queue.\n
 * In the case of OpenGL this simply swaps the buffers.
 * @param instance RendererInstance to present the frame.
 */
void renderer_present(RendererInstance* instance);
