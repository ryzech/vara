#pragma once

#include <vara/core/defines.h>

typedef struct RenderPassConfig {
    const char* name;
} RenderPassConfig;

struct RenderPass;
struct RendererInstance;
struct Buffer;

typedef struct RenderPassVT {
    b8 (*render_pass_create)(
        struct RenderPass* pass, const RenderPassConfig* config
    );
    void (*render_pass_destroy)(struct RenderPass* pass);
    void (*render_pass_begin)(struct RenderPass* pass);
    void (*render_pass_draw_indexed)(
        struct RenderPass* pass, Buffer* index_buffer
    );
    void (*render_pass_end)(struct RenderPass* pass);
} RenderPassVT;

typedef struct RenderPass {
    const char* name;
    RendererInstance* renderer;
    RenderPassConfig* config;
    RenderPassVT vt;
    void* backend_data;
    b8 is_recording;
} RenderPass;

RenderPass* render_pass_create(
    RendererInstance* instance, const RenderPassConfig* config
);

void render_pass_destroy(RenderPass* pass);

void render_pass_begin(RenderPass* pass);

void render_pass_draw_indexed(RenderPass* pass, Buffer* vertex_buffer, Buffer* index_buffer);

void render_pass_end(RenderPass* pass);
