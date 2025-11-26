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
    // Should likely move this into config somewhere, or be pass specific?
    glEnable(GL_DEPTH_TEST);
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

static void renderer_opengl_set_viewport(Vector2i position, Vector2i size) {
    glViewport(position.x, position.y, size.x, size.y);
}

static void renderer_opengl_present(void) {
    platform_window_swap_buffers(renderer_state.window);
}

static void renderer_opengl_destroy(void) {
}

void renderer_opengl_init(RendererInstance* instance, VaraWindow* window) {
    renderer_state.window = window;

    instance->name = "OpenGL";
    instance->vt.renderer_create = renderer_opengl_create;
    instance->vt.renderer_clear = renderer_opengl_clear;
    instance->vt.renderer_clear_color = renderer_opengl_clear_color;
    instance->vt.renderer_set_viewport = renderer_opengl_set_viewport;
    instance->vt.renderer_present = renderer_opengl_present;
    instance->vt.renderer_destroy = renderer_opengl_destroy;

    DEBUG("Creating RendererInstance named('%s')", instance->name);
}
