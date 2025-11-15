#include <glad/gl.h>

#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/renderer.h>
#include "vara/core/platform/platform.h"

typedef struct OpenGLRendererState {
    VaraWindow* window;
} OpenGLRendererState;

static OpenGLRendererState renderer_state;

static b8 renderer_opengl_create(void) {
    gladLoadGL((GLADloadfunc)platform_window_get_proc_address);
    glEnable(GL_DEBUG_OUTPUT);
    DEBUG(
        "Loaded OpenGL: %s | %s",
        glGetString(GL_VERSION),
        glGetString(GL_RENDERER)
    );
    return true;
}

// Do we need glClear and glCLearColor? Should they be differentiated?
static void renderer_opengl_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Do we need glClear and glCLearColor? Should they be differentiated?
static void renderer_opengl_clear_color(Vector4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

static void renderer_opengl_present(void) {
    platform_window_swap_buffers(renderer_state.window);
}

static void renderer_opengl_destroy(void) {
}

RendererInstance* renderer_opengl_init(VaraWindow* window) {
    renderer_state.window = window;

    RendererInstance* opengl_instance =
        platform_allocate(sizeof(RendererInstance));
    opengl_instance->name = "OpenGL";
    opengl_instance->renderer_type = renderer_state.window->graphics_type;
    opengl_instance->vt.renderer_create = renderer_opengl_create;
    opengl_instance->vt.renderer_clear = renderer_opengl_clear;
    opengl_instance->vt.renderer_clear_color = renderer_opengl_clear_color;
    opengl_instance->vt.renderer_present = renderer_opengl_present;
    opengl_instance->vt.renderer_destroy = renderer_opengl_destroy;

    DEBUG("Creating RendererInstance named('%s')", opengl_instance->name);

    return opengl_instance;
}
