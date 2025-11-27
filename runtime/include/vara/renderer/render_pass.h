#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/math.h>

// Forward declarations, in case they need access to each other.
typedef struct RenderPassVT RenderPassVT;
typedef struct RenderPass RenderPass;
typedef struct RenderPassConfig RenderPassConfig;

// Redeclaration's so we don't have to include header.
struct Framebuffer;
struct Buffer;
struct Shader;
struct RenderCommandBuffer;

struct RenderPassConfig {
    Vector4 clear_color;
    const char* name;
    struct Framebuffer* target;
    // TODO: LOAD/STORE ops rather than just clear.
    b8 clear;
};

struct RenderPassVT {
    b8 (*render_pass_create)(RenderPass* pass, const RenderPassConfig* config);
    void (*render_pass_destroy)(RenderPass* pass);
    void (*render_pass_begin)(RenderPass* pass);
    void (*render_pass_draw_indexed)(RenderPass* pass, struct Buffer* index_buffer);
    void (*render_pass_end)(RenderPass* pass);
};

struct RenderPass {
    RenderPassVT vt;
    Vector4 clear_color;
    const char* name;
    struct Framebuffer* target;
    void* backend_data;
    b8 clear;
};

RenderPass* render_pass_create(const RenderPassConfig* config);
void render_pass_destroy(RenderPass* pass);

void render_pass_begin(RenderPass* pass);
void render_pass_draw_indexed(
    RenderPass* pass,
    struct Shader* shader,
    struct Buffer* vertex_buffer,
    struct Buffer* index_buffer
);
void render_pass_shader_set_mat4(
    RenderPass* pass, struct Shader* shader, const char* name, Matrix4 matrix
);
void render_pass_end(RenderPass* pass);
