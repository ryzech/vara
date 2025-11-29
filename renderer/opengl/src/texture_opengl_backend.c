#include <glad/gl.h>
#include <vara/core/platform/platform.h>
#include <vara/renderer/texture.h>

typedef struct OpenGLTextureState {
    GLuint id;
    GLuint slot;
} OpenGLTextureState;

static GLenum texture_format_to_gl(TextureFormat format) {
    switch (format) {
        case TEXTURE_FORMAT_RGB:
            return GL_RGB;
        case TEXTURE_FORMAT_RGBA:
            return GL_RGBA;
    }
    return GL_RGBA;
}

static GLint texture_format_to_internal_gl(TextureFormat format) {
    switch (format) {
        case TEXTURE_FORMAT_RGB:
            return GL_RGB8;
        case TEXTURE_FORMAT_RGBA:
            return GL_RGBA8;
    }
    return GL_RGBA8;
}

static GLint texture_filter_to_gl(TextureFilter filter) {
    switch (filter) {
        case TEXTURE_FILTER_LINEAR:
            return GL_LINEAR;
        case TEXTURE_FILTER_NEAREST:
            return GL_NEAREST;
    }
    return GL_LINEAR;
}

b8 texture_opengl_create(Texture* texture, const TextureConfig* config) {
    if (!texture || !config) {
        return false;
    }

    OpenGLTextureState* texture_state = platform_allocate(sizeof(OpenGLTextureState));
    platform_zero_memory(texture_state, sizeof(OpenGLTextureState));
    if (!texture_state) {
        return false;
    }

    GLint internal = texture_format_to_internal_gl(config->format);
    GLenum format = texture_format_to_gl(config->format);
    GLint filter = texture_filter_to_gl(config->filter);

    glGenTextures(1, &texture_state->id);
    glBindTexture(GL_TEXTURE_2D, texture_state->id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal,
        (GLsizei)config->width,
        (GLsizei)config->height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    texture->backend_data = texture_state;
    return true;
}

void texture_opengl_destroy(Texture* texture) {
    if (!texture || !texture->backend_data) {
        return;
    }

    OpenGLTextureState* texture_state = texture->backend_data;

    glDeleteTextures(1, &texture_state->id);

    platform_free(texture_state);
    texture->backend_data = NULL;
}

void texture_opengl_bind(Texture* texture, u32 slot) {
    if (!texture || !texture->backend_data) {
        return;
    }

    OpenGLTextureState* texture_state = texture->backend_data;
    texture_state->slot = slot;

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_state->id);
}

void texture_opengl_unbind(Texture* texture) {
    if (!texture) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_opengl_set_data(Texture* texture, void* data, size_t size) {
    if (!texture || !texture->backend_data) {
        return;
    }

    OpenGLTextureState* texture_state = texture->backend_data;

    GLenum format = texture_format_to_gl(texture->format);

    glBindTexture(GL_TEXTURE_2D, texture_state->id);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        (GLsizei)texture->width,
        (GLsizei)texture->height,
        format,
        GL_UNSIGNED_BYTE,
        data
    );
}

u32 texture_opengl_get_id(Texture* texture) {
    if (!texture || !texture->backend_data) {
        return 0;
    }

    OpenGLTextureState* texture_state = texture->backend_data;

    return texture_state->id;
}

void texture_opengl_init(Texture* texture) {
    texture->vt.texture_create = texture_opengl_create;
    texture->vt.texture_destroy = texture_opengl_destroy;
    texture->vt.texture_bind = texture_opengl_bind;
    texture->vt.texture_unbind = texture_opengl_unbind;
    texture->vt.texture_set_data = texture_opengl_set_data;
    texture->vt.texture_get_id = texture_opengl_get_id;
}
