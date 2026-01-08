#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/render_pipeline_opengl_backend.h"
#include "vara/renderer/shader_opengl_backend.h"

static GLenum compare_op_to_gl(CompareOp op) {
    switch (op) {
        case COMPARE_NEVER:
            return GL_NEVER;
        case COMPARE_LESS:
            return GL_LESS;
        case COMPARE_EQUAL:
            return GL_EQUAL;
        case COMPARE_LESS_OR_EQUAL:
            return GL_LEQUAL;
        case COMPARE_GREATER:
            return GL_GREATER;
        case COMPARE_NOT_EQUAL:
            return GL_NOTEQUAL;
        case COMPARE_GREATER_OR_EQUAL:
            return GL_GEQUAL;
        case COMPARE_ALWAYS:
            return GL_ALWAYS;
        default:
            return GL_LESS;
    }
}

static GLenum blend_factor_to_gl(BlendFactor factor) {
    switch (factor) {
        case BLEND_ZERO:
            return GL_ZERO;
        case BLEND_ONE:
            return GL_ONE;
        case BLEND_SRC_COLOR:
            return GL_SRC_COLOR;
        case BLEND_ONE_MINUS_SRC_COLOR:
            return GL_ONE_MINUS_SRC_COLOR;
        case BLEND_DST_COLOR:
            return GL_DST_COLOR;
        case BLEND_ONE_MINUS_DST_COLOR:
            return GL_ONE_MINUS_DST_COLOR;
        case BLEND_SRC_ALPHA:
            return GL_SRC_ALPHA;
        case BLEND_ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        case BLEND_DST_ALPHA:
            return GL_DST_ALPHA;
        case BLEND_ONE_MINUS_DST_ALPHA:
            return GL_ONE_MINUS_DST_ALPHA;
        default:
            return GL_ONE;
    }
}

typedef struct OpenGLRenderPipelineState {
    b8 depth_test, depth_write;
    GLenum depth_func;
    b8 blend_enable;
    GLenum blend_src, blend_dest;
} OpenGLRenderPipelineState;

static OpenGLRenderPipelineState* previous_state;

b8 render_pipeline_opengl_create(RenderPipeline* pipeline, const RenderPipelineConfig* config) {
    if (!pipeline || !config) {
        return false;
    }

    DEBUG("Creating RenderPipeline named('%s')", config->name);
    OpenGLRenderPipelineState* state = platform_allocate(sizeof(OpenGLRenderPipelineState));
    if (!state) {
        return false;
    }

    state->depth_test = pipeline->depth.test;
    state->depth_write = pipeline->depth.write;
    state->depth_func = compare_op_to_gl(pipeline->depth.compare);

    state->blend_enable = pipeline->blend.enable;
    state->blend_src = blend_factor_to_gl(pipeline->blend.src);
    state->blend_dest = blend_factor_to_gl(pipeline->blend.dst);

    pipeline->backend_data = state;

    return true;
}

void render_pipeline_opengl_destroy(RenderPipeline* pipeline) {
    if (!pipeline || !pipeline->backend_data) {
        return;
    }

    platform_free(pipeline->backend_data);
    pipeline->backend_data = NULL;
}

void render_pipeline_opengl_bind(RenderPipeline* pipeline) {
    if (!pipeline || !pipeline->backend_data) {
        return;
    }

    OpenGLRenderPipelineState* state = pipeline->backend_data;
    shader_opengl_bind(pipeline->shader);

    if (state != previous_state) {
        if (state->depth_test) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(state->depth_func);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        glDepthMask(state->depth_write ? GL_TRUE : GL_FALSE);

        if (state->blend_enable) {
            glEnable(GL_BLEND);
            glBlendFunc(state->blend_src, state->blend_dest);
        } else {
            glDisable(GL_BLEND);
        }

        previous_state = state;
    }
}
