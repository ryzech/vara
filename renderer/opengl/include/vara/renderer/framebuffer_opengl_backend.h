#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/framebuffer.h>

b8 framebuffer_opengl_create(Framebuffer* buffer, const FramebufferConfig* config);
void framebuffer_opengl_destroy(Framebuffer* buffer);
void framebuffer_opengl_bind(Framebuffer* buffer);
void framebuffer_opengl_unbind(Framebuffer* buffer);
void framebuffer_opengl_resize(Framebuffer* buffer, u32 width, u32 height);