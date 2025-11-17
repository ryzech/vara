#pragma once

#include <vara/core/defines.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/renderer.h"

typedef struct RenderPassVT RenderPassVT;
typedef struct RenderPass RenderPass;
typedef struct RenderPassConfig RenderPassConfig;

struct RenderPassConfig {
    const char* name;
};

struct RenderPassVT {
    b8 (*render_pass_create)(RenderPass* pass, const RenderPassConfig* config);
    void (*render_pass_destroy)(RenderPass* pass);
    void (*render_pass_begin)(RenderPass* pass);
    void (*render_pass_draw_indexed)(RenderPass* pass, Buffer* index_buffer);
    void (*render_pass_end)(RenderPass* pass);
};

struct RenderPass {
    const char* name;
    RendererInstance* renderer;
    RenderPassConfig* config;
    RenderPassVT vt;
    void* backend_data;
    b8 is_recording;
};

RenderPass* render_pass_create(
    const RendererInstance* instance, const RenderPassConfig* config
);

void render_pass_destroy(RenderPass* pass);

void render_pass_begin(RenderPass* pass);

void render_pass_draw_indexed(
    RenderPass* pass, Buffer* vertex_buffer, Buffer* index_buffer
);

void render_pass_end(RenderPass* pass);
