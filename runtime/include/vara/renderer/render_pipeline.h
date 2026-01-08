#pragma once

#include <vara/core/defines.h>
#include "vara/renderer/render_types.h"

typedef struct RenderPipeline RenderPipeline;
typedef struct RenderPipelineConfig RenderPipelineConfig;

struct Shader;
struct VertexLayout;
struct Framebuffer;
struct Renderer;
struct RendererBackend;

struct RenderPipelineConfig {
    const char* name;
    struct Shader* shader;
    struct VertexLayout* layout;
    DepthState depth;
    BlendState blend;
};

struct RenderPipeline {
    const char* name;
    struct Shader* shader;
    struct VertexLayout* layout;
    DepthState depth;
    BlendState blend;
    void* backend_data;
    struct RendererBackend* backend;
};

RenderPipeline* render_pipeline_create(
    struct Renderer* renderer, const RenderPipelineConfig* config
);
void render_pipeline_destroy(RenderPipeline* pipeline);