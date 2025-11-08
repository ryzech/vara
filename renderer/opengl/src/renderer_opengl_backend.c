#include <glad/gl.h>

#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/renderer.h>

static b8 renderer_opengl_create(void) {
    i32 version = gladLoadGL((GLADloadfunc)platform_window_get_proc_address);
    DEBUG(
        "Loaded OpenGL: %s | %s",
        glGetString(GL_VERSION),
        glGetString(GL_RENDERER)
    );
    return true;
}

static void renderer_opengl_destroy(void) {
}

static RendererInstance opengl_instance = {
    .name = "OpenGL",
    .renderer_create = renderer_opengl_create,
    .renderer_destroy = renderer_opengl_destroy
};

RendererInstance* renderer_opengl_init(void) {
    DEBUG("Creating RendererInstance named('%s')", opengl_instance.name);
    return &opengl_instance;
}
