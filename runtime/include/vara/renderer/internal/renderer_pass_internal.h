#pragma once

#include "vara/renderer/render_pass.h"

typedef struct RenderPassBackendVT RenderPassBackendVT;

struct RenderPassBackendVT {
    b8 (*create)(RenderPass* pass, const RenderPassConfig* config);
    void (*destroy)(RenderPass* pass);
    void (*begin)(RenderPass* pass);
    void (*end)(RenderPass* pass);
};