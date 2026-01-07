#pragma once

#include <vara/core/math/types.h>
#include <vara/core/platform/platform_graphics_types.h>

#include "vara/renderer/internal/renderer_buffer_internal.h"
#include "vara/renderer/internal/renderer_framebuffer_internal.h"
#include "vara/renderer/internal/renderer_pass_internal.h"
#include "vara/renderer/internal/renderer_pipeline_internal.h"
#include "vara/renderer/internal/renderer_shader_internal.h"
#include "vara/renderer/internal/renderer_texture_internal.h"
#include "vara/renderer/render_command.h"

typedef struct RendererBackend RendererBackend;
typedef struct RendererBackendVT RendererBackendVT;

struct VaraWindow;
struct Renderer;

struct RendererBackendVT {
    b8 (*create)(void);
    void (*destroy)(void);
    void (*set_viewport)(Vector2i viewport_size);
    void (*submit)(const RenderCommandBuffer* buffer);
    void (*present)(void);
};

struct RendererBackend {
    RendererBackendVT renderer;
    ShaderBackendVT shader;
    BufferBackendVT buffer;
    FramebufferBackendVT framebuffer;
    RenderPassBackendVT render_pass;
    RenderPipelineBackendVT render_pipeline;
    TextureBackendVT texture;

    const char* name;
    PlatformRendererType type;
};

RendererBackend* renderer_backend_create(struct VaraWindow* window);
void renderer_backend_destroy(RendererBackend* backend);

RendererBackend* renderer_backend_get(struct Renderer* renderer);