#pragma once

#include "vara/renderer/render_pipeline.h"

typedef struct RenderPipelineBackendVT RenderPipelineBackendVT;

struct RenderPipelineBackendVT {
    b8 (*create)(RenderPipeline* pipeline, const RenderPipelineConfig* config);
    void (*destroy)(RenderPipeline* pipeline);
    void (*bind)(RenderPipeline* pipeline);
};