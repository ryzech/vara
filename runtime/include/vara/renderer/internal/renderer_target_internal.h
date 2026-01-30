#pragma once

#include "vara/renderer/render_target.h"

typedef struct RenderTargetBackendVT RenderTargetBackendVT;

struct RenderTargetBackendVT {
    b8 (*create)(RenderTarget* target, const RenderTargetConfig* config);
    void (*destroy)(RenderTarget* target);
    void (*resize)(RenderTarget* target, u32 width, u32 height);
};