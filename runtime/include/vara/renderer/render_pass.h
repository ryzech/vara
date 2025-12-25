#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

// Forward declarations, in case they need access to each other.
typedef struct RenderPass RenderPass;
typedef struct RenderPassConfig RenderPassConfig;

// Redeclaration's so we don't have to include header.
struct Framebuffer;
struct RenderCommandBuffer;
struct Renderer;
struct RendererBackend;

struct RenderPassConfig {
    Vector4 clear_color;
    const char* name;
    struct Framebuffer* target;
    // TODO: LOAD/STORE ops rather than just clear.
    b8 clear;
};

struct RenderPass {
    Vector4 clear_color;
    const char* name;
    struct Framebuffer* target;
    void* backend_data;
    b8 clear;
    struct RendererBackend* backend;
};

RenderPass* render_pass_create(struct Renderer* renderer, const RenderPassConfig* config);
void render_pass_destroy(RenderPass* pass);

void render_pass_begin(struct Renderer* renderer, RenderPass* pass);
void render_pass_end(struct Renderer* renderer, RenderPass* pass);
