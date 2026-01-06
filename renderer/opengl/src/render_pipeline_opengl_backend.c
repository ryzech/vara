#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/render_pipeline_opengl_backend.h"

#include "vara/renderer/shader_opengl_backend.h"

typedef struct OpenGLRenderPipelineState {
} OpenGLRenderPipelineState;

b8 render_pipeline_opengl_create(RenderPipeline* pipeline, const RenderPipelineConfig* config) {
    DEBUG("Creating RenderPipeline named('%s')", config->name);
    OpenGLRenderPipelineState* render_pipeline_state =
        platform_allocate(sizeof(OpenGLRenderPipelineState));
    if (!render_pipeline_state) {
        return false;
    }

    pipeline->backend_data = render_pipeline_state;

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

    OpenGLRenderPipelineState* render_pipeline_state = pipeline->backend_data;
    shader_opengl_bind(pipeline->shader);
}
