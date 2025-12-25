#pragma once

typedef struct RenderContext RenderContext;

struct Renderer2D;
struct Renderer;

struct RenderContext {
    struct Renderer2D* r2d;
};

RenderContext* render_context_create(struct Renderer* renderer);
void render_context_destroy(RenderContext* context);

void render_context_begin_frame(RenderContext* context);
void render_context_end_frame(RenderContext* context);