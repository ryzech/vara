#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/buffer.h>

b8 buffer_opengl_create(Buffer* buffer, const BufferConfig* config);
void buffer_opengl_destroy(Buffer* buffer);
void buffer_opengl_bind(Buffer* buffer);
void buffer_opengl_unbind(Buffer* buffer);
void buffer_opengl_set_data(Buffer* buffer, const void* data, size_t size, size_t offset);