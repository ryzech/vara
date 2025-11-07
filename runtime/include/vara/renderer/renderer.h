#pragma once

#include <vara/core/defines.h>

/* RendererInstance holds pointers to the functions implementated
 * by each Renderer API. 
 *
 * This is so muktiple renderers can be supported
 * and users can select (for example on Steam) a different one if there are 
 * any issues encountered. */
typedef struct RendererInstance {
    const char* name;
    b8 (*renderer_create)(void);
    void (*renderer_destroy)(void);
} RendererInstance;

/* Helper function to call the instances function pointer. */
static inline b8 renderer_create(RendererInstance* instance) {
    return instance->renderer_create();
}

/* Helper function to call the instances function pointer. */
static inline void renderer_destroy(RendererInstance* instance) {
    return instance->renderer_destroy();
}

/* Exported function to initialize a specific API, returns the instance
 * with the implementated function pointers. */
RendererInstance* renderer_opengl_init(void);
