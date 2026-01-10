#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/framebuffer_opengl_backend.h"
#include "vara/renderer/render_pass_opengl_backend.h"

typedef struct OpenGLRenderPassState {
    b8 active;
} OpenGLRenderPassState;

b8 render_pass_opengl_create(RenderPass* pass, const RenderPassConfig* config) {
    DEBUG("Creating render pass named('%s')", config->name);
    OpenGLRenderPassState* render_pass_state = vara_allocate(sizeof(OpenGLRenderPassState));
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

    vara_free(pass->backend_data, sizeof(OpenGLRenderPassState));
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

    if (pass->target) {
        framebuffer_opengl_bind(pass->target);
    }

    for (u32 i = 0; i < pass->color_attachment_count; i++) {
        const RenderPassAttachment* attachment = &pass->color_attachments[i];
        switch (attachment->load) {
            case ATTACHMENT_LOAD_OP_CLEAR: {
                const Vector4 value = attachment->clear;
                glClearColor(value.x, value.y, value.z, value.w);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            }
            case ATTACHMENT_LOAD_OP_LOAD: {
                break;
            }
            case ATTACHMENT_LOAD_OP_DONT_CARE: {
                break;
            }
        }
    }

    // TODO: handle depth/stencil.

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

    if (pass->target) {
        framebuffer_opengl_unbind(pass->target);
    }

    render_pass_state->active = false;
}
