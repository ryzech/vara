#include <glad/gl.h>
#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/renderer/render_pass.h>

typedef struct OpenGLRenderPassState {
    b8 active;
} OpenGLRenderPassState;

b8 render_pass_opengl_create(RenderPass* pass, const RenderPassConfig* config) {
    DEBUG("Creating render pass named('%s')", config->name);
    OpenGLRenderPassState* render_pass_state = platform_allocate(sizeof(OpenGLRenderPassState));
    if (!render_pass_state) {
        return false;
    }

    render_pass_state->active = false;
    pass->backend_data = render_pass_state;

    return true;
}

void render_pass_opengl_destroy(RenderPass* pass) {
    if (!pass || !pass->backend_data) {
        return;
    }

    platform_free(pass->backend_data);
    pass->backend_data = NULL;
}

void render_pass_opengl_begin(RenderPass* pass) {
    if (!pass || !pass->backend_data) {
        return;
    }

    OpenGLRenderPassState* render_pass_state = pass->backend_data;

    if (render_pass_state->active) {
        WARN("Render pass named('%s') is already active!", pass->name);
        return;
    }

    render_pass_state->active = true;
}

void render_pass_opengl_end(RenderPass* pass) {
    if (!pass || !pass->backend_data) {
        return;
    }

    OpenGLRenderPassState* render_pass_state = pass->backend_data;

    if (!render_pass_state->active) {
        WARN("Render pass named('%s') is not active!", pass->name);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    render_pass_state->active = false;
}
