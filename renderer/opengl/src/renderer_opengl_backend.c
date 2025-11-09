#include <glad/gl.h>

#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/renderer.h>

typedef struct OpenGLRendererState {
    VaraWindow* window;
} OpenGLRendererState;

static OpenGLRendererState renderer_state;

static b8 renderer_opengl_create(void) {
    i32 version = gladLoadGL((GLADloadfunc)platform_window_get_proc_address);
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

static RendererInstance opengl_instance = {
    .name = "OpenGL",
    .vt = {
        .renderer_create = renderer_opengl_create,
        .renderer_clear = renderer_opengl_clear,
        .renderer_clear_color = renderer_opengl_clear_color,
        .renderer_present = renderer_opengl_present,
        .renderer_destroy = renderer_opengl_destroy
    }
};

RendererInstance* renderer_opengl_init(VaraWindow* window) {
    DEBUG("Creating RendererInstance named('%s')", opengl_instance.name);

    renderer_state.window = window;

    return &opengl_instance;
}
