#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/render_pass.h>

b8 render_pass_opengl_create(RenderPass* pass, const RenderPassConfig* config);
void render_pass_opengl_destroy(RenderPass* pass);
void render_pass_opengl_begin(RenderPass* pass);
void render_pass_opengl_end(RenderPass* pass);