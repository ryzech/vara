#pragma once

#include <vara/core/defines.h>

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
    struct Framebuffer* target;
};

struct RenderPipeline {
    const char* name;
    struct Shader* shader;
    struct VertexLayout* layout;
    struct Framebuffer* target;
    void* backend_data;
    struct RendererBackend* backend;
};

RenderPipeline* render_pipeline_create(
    struct Renderer* renderer, const RenderPipelineConfig* config
);
void render_pipeline_destroy(RenderPipeline* pipeline);