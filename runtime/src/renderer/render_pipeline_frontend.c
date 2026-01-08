#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_pipeline.h"
#include "vara/renderer/renderer.h"

RenderPipeline* render_pipeline_create(Renderer* renderer, const RenderPipelineConfig* config) {
    RenderPipeline* pipeline = platform_allocate(sizeof(RenderPipeline));
    platform_zero_memory(pipeline, sizeof(RenderPipeline));

    pipeline->name = config->name;
    pipeline->shader = config->shader;
    pipeline->layout = config->layout;
    pipeline->depth = config->depth;
    pipeline->blend = config->blend;

    RendererBackend* backend = renderer_backend_get(renderer);
    pipeline->backend = backend;

    if (!pipeline->backend->render_pipeline.create(pipeline, config)) {
        ERROR("Failed to create RenderPipeline named('%s')", config->name);
        render_pipeline_destroy(pipeline);
        return NULL;
    }

    return pipeline;
}

void render_pipeline_destroy(RenderPipeline* pipeline) {
    if (pipeline) {
        pipeline->backend->render_pipeline.destroy(pipeline);
        platform_free(pipeline);
    }
}