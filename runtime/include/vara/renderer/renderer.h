#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>
#include "vara/core/platform/platform_window.h"

typedef struct RendererInstanceVT {
    b8 (*renderer_create)(void);
    void (*renderer_destroy)(void);
    void (*renderer_clear)(void);
    void (*renderer_clear_color)(Vector4 color);
    void (*renderer_present)(void);
} RendererInstanceVT;

/* RendererInstance holds pointers to the functions implementated
 * by each Renderer API. 
 *
 * This is so muktiple renderers can be supported
 * and users can select (for example on Steam) a different one if there are 
 * any issues encountered. */
typedef struct RendererInstance {
    const char* name;
    RendererInstanceVT vt;
} RendererInstance;

RendererInstance* renderer_create(VaraWindow* window);

void renderer_destroy(RendererInstance* instance);

/* Helper function to call the instances function pointer. */
void renderer_clear(RendererInstance* instance);

/* Helper function to call the instances function pointer. */
void renderer_clear_color(RendererInstance* instance, Vector4 color);

/* Helper function to call the instances function pointer. */
void renderer_present(RendererInstance* instance);

/* Exported function to initialize a specific API, returns the instance
 * with the implementated function pointers. */
RendererInstance* renderer_opengl_init(VaraWindow* window);
