#pragma once

#include <vara/core/defines.h>

// Forward declarations, in case they need access to each other.
typedef struct RenderPassVT RenderPassVT;
typedef struct RenderPass RenderPass;
typedef struct RenderPassConfig RenderPassConfig;

// Redeclaration's so we don't have to include header.
struct RendererInstance;
struct Framebuffer;
struct Buffer;
struct Shader;

struct RenderPassConfig {
    const char* name;
    struct Framebuffer* target;
};

struct RenderPassVT {
    b8 (*render_pass_create)(RenderPass* pass, const RenderPassConfig* config);
    void (*render_pass_destroy)(RenderPass* pass);
    void (*render_pass_begin)(RenderPass* pass);
    void (*render_pass_draw_indexed)(
        RenderPass* pass, struct Buffer* index_buffer
    );
    void (*render_pass_end)(RenderPass* pass);
};

struct RenderPass {
    const char* name;
    struct RendererInstance* renderer;
    RenderPassConfig* config;
    RenderPassVT vt;
    void* backend_data;
    b8 is_recording;
};

RenderPass* render_pass_create(
    const struct RendererInstance* instance, const RenderPassConfig* config
);

void render_pass_destroy(RenderPass* pass);

void render_pass_begin(RenderPass* pass);

void render_pass_draw_indexed(
    RenderPass* pass,
    struct Shader* shader,
    struct Buffer* vertex_buffer,
    struct Buffer* index_buffer
);

void render_pass_end(RenderPass* pass);
