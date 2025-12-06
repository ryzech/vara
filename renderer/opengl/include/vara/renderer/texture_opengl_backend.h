#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/texture.h>

typedef struct OpenGLTextureState {
    GLuint id;
    GLuint slot;
} OpenGLTextureState;

b8 texture_opengl_create(Texture* texture, const TextureConfig* config);
void texture_opengl_destroy(Texture* texture);
void texture_opengl_bind(Texture* texture, u32 slot);
void texture_opengl_unbind(Texture* texture);
void texture_opengl_set_data(Texture* texture, void* data, size_t size);
u32 texture_opengl_get_id(Texture* texture);