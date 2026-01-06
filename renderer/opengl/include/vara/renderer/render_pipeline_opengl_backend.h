#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/render_pipeline.h>

b8 render_pipeline_opengl_create(RenderPipeline* pipeline, const RenderPipelineConfig* config);
void render_pipeline_opengl_destroy(RenderPipeline* pipeline);
void render_pipeline_opengl_bind(RenderPipeline* pipeline);