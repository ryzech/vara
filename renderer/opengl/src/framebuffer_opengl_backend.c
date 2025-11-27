#include <glad/gl.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/renderer/framebuffer.h>

typedef struct OpenGLFramebufferState {
    GLuint fbo;
    GLuint* color_buffers;
    u32 color_buffer_count;
    GLuint depth_buffer;
    GLuint stencil_buffer;
    GLuint depth_stencil_buffer;
} OpenGLFramebufferState;

static GLint format_to_gl_internal(FramebufferAttachmentFormat format) {
    switch (format) {
        case FRAMEBUFFER_FORMAT_RGBA8:
            return GL_RGBA8;
        case FRAMEBUFFER_FORMAT_DEPTH24_STENCIL8:
            return GL_DEPTH24_STENCIL8;
        default:
            return 0;
    }
}

static GLenum format_to_gl_format(FramebufferAttachmentFormat format) {
    switch (format) {
        case FRAMEBUFFER_FORMAT_RGBA8:
            return GL_RGBA;
        case FRAMEBUFFER_FORMAT_DEPTH24_STENCIL8:
            return GL_DEPTH_STENCIL;
        default:
            return 0;
    }
}

static GLenum format_to_gl_type(FramebufferAttachmentFormat format) {
    switch (format) {
        case FRAMEBUFFER_FORMAT_DEPTH24_STENCIL8:
            return GL_UNSIGNED_INT_24_8;
        default:
            return GL_UNSIGNED_BYTE;
    }
}

static GLuint opengl_texture_create(
    const FramebufferAttachmentConfig* config, u32 width, u32 height, u32 samples
) {
    GLuint texture;
    glGenTextures(1, &texture);

    GLenum target = (samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glBindTexture(target, texture);

    GLint internal_format = format_to_gl_internal(config->format);
    GLenum format = format_to_gl_format(config->format);
    GLenum type = format_to_gl_type(config->format);

    if (samples > 1) {
        glTexImage2DMultisample(
            target, (GLsizei)samples, internal_format, (GLsizei)width, (GLsizei)height, GL_TRUE
        );
    } else {
        glTexImage2D(
            target, 0, internal_format, (GLsizei)width, (GLsizei)height, 0, format, type, NULL
        );
    }

    if (samples <= 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glBindTexture(target, 0);
    return texture;
}

static b8 framebuffer_opengl_create(Framebuffer* buffer, const FramebufferConfig* config) {
    if (!buffer || !config) {
        return false;
    }

    OpenGLFramebufferState* buffer_state = platform_allocate(sizeof(OpenGLFramebufferState));
    platform_zero_memory(buffer_state, sizeof(OpenGLFramebufferState));
    if (!buffer_state) {
        return false;
    }

    glGenFramebuffers(1, &buffer_state->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer_state->fbo);

    u32 color_count = 0;
    for (u32 i = 0; i < config->attachment_count; i++) {
        if (config->attachments[i].type == FRAMEBUFFER_ATTACHMENT_COLOR) {
            color_count++;
        }
    }

    if (color_count > 0) {
        buffer_state->color_buffers = platform_allocate(sizeof(GLuint) * color_count);
        buffer_state->color_buffer_count = color_count;
    }

    u32 color_index = 0;
    for (u32 i = 0; i < config->attachment_count; i++) {
        const FramebufferAttachmentConfig* attachment = &config->attachments[i];
        GLuint texture =
            opengl_texture_create(attachment, config->width, config->height, config->samples);
        GLenum target = (config->samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

        switch (attachment->type) {
            case FRAMEBUFFER_ATTACHMENT_COLOR: {
                GLenum color = GL_COLOR_ATTACHMENT0 + color_index;
                glFramebufferTexture2D(GL_FRAMEBUFFER, color, target, texture, 0);
                buffer_state->color_buffers[color_index++] = texture;
                break;
            }
            case FRAMEBUFFER_ATTACHMENT_DEPTH:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, texture, 0);
                buffer_state->depth_buffer = texture;
                break;
            case FRAMEBUFFER_ATTACHMENT_STENCIL:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, target, texture, 0);
                buffer_state->stencil_buffer = texture;
                break;
            case FRAMEBUFFER_ATTACHMENT_DEPTH_STENCIL:
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target, texture, 0
                );
                buffer_state->depth_stencil_buffer = texture;
                break;
        }
    }

    if (color_count > 0) {
        GLenum* draw_buffers = platform_allocate(sizeof(GLenum) * color_count);
        for (u32 i = 0; i < color_count; i++) {
            draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers((GLsizei)color_count, draw_buffers);
        platform_free(draw_buffers);
    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("Framebuffer named('%s') is not complete!", config->name);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer->backend_data = buffer_state;
    buffer->width = config->width;
    buffer->height = config->height;
    buffer->samples = config->samples;

    return true;
}

static void framebuffer_opengl_destroy(Framebuffer* buffer) {
    if (!buffer || !buffer->backend_data) {
        return;
    }

    OpenGLFramebufferState* buffer_state = buffer->backend_data;

    if (buffer_state->color_buffers) {
        glDeleteTextures((GLsizei)buffer_state->color_buffer_count, buffer_state->color_buffers);
        platform_free(buffer_state->color_buffers);
    }

    if (buffer_state->depth_buffer) {
        glDeleteTextures(1, &buffer_state->depth_buffer);
    }

    if (buffer_state->stencil_buffer) {
        glDeleteTextures(1, &buffer_state->stencil_buffer);
    }

    if (buffer_state->depth_stencil_buffer) {
        glDeleteTextures(1, &buffer_state->depth_stencil_buffer);
    }

    // This should always exist but to be safe we'll check.
    if (buffer_state->fbo) {
        glDeleteFramebuffers(1, &buffer_state->fbo);
    }

    platform_free(buffer_state);
    buffer->backend_data = NULL;
}

static void framebuffer_opengl_bind(Framebuffer* buffer) {
    if (!buffer || !buffer->backend_data) {
        return;
    }

    OpenGLFramebufferState* buffer_state = buffer->backend_data;

    glBindFramebuffer(GL_FRAMEBUFFER, buffer_state->fbo);
}

static void framebuffer_opengl_unbind(Framebuffer* buffer) {
    if (!buffer) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void framebuffer_opengl_resize(Framebuffer* buffer, u32 width, u32 height) {
    if (!buffer) {
        return;
    }

    framebuffer_opengl_destroy(buffer);

    buffer->width = width;
    buffer->height = height;

    const FramebufferConfig config = {
        .name = buffer->name,
        .width = width,
        .height = height,
        .attachments = buffer->attachments,
        .attachment_count = buffer->attachment_count,
        .samples = buffer->samples
    };

    framebuffer_opengl_create(buffer, &config);
}

void framebuffer_opengl_init(Framebuffer* buffer) {
    buffer->vt.framebuffer_create = framebuffer_opengl_create;
    buffer->vt.framebuffer_destroy = framebuffer_opengl_destroy;
    buffer->vt.framebuffer_bind = framebuffer_opengl_bind;
    buffer->vt.framebuffer_unbind = framebuffer_opengl_unbind;
    buffer->vt.framebuffer_resize = framebuffer_opengl_resize;
}