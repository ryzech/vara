#include <glad/gl.h>
#include <vara/core/memory/memory.h>

#include "vara/renderer/buffer_opengl_backend.h"

typedef struct OpenGLBufferState {
    GLuint id;
    GLenum target;
    GLuint vao;
} OpenGLBufferState;

static GLenum type_to_gl_type(BufferType type) {
    switch (type) {
        case BUFFER_TYPE_VERTEX:
            return GL_ARRAY_BUFFER;
        case BUFFER_TYPE_INDEX:
            return GL_ELEMENT_ARRAY_BUFFER;
        case BUFFER_TYPE_UNIFORM:
            return GL_UNIFORM_BUFFER;
        default:
            return GL_ARRAY_BUFFER;
    }
}

static GLenum usage_to_gl_usage(BufferUsage usage) {
    switch (usage) {
        case BUFFER_USAGE_STATIC:
            return GL_STATIC_DRAW;
        case BUFFER_USAGE_DYNAMIC:
            return GL_DYNAMIC_DRAW;
        case BUFFER_USAGE_STREAM:
            return GL_STREAM_DRAW;
        default:
            return GL_STATIC_DRAW;
    }
}

static void attribute_to_gl_attribute(
    VertexAttributeType type, GLenum* out_gl_type, GLint* out_count
) {
    switch (type) {
        case VERTEX_ATTRIBUTE_FLOAT:
            *out_gl_type = GL_FLOAT;
            *out_count = 1;
            break;
        case VERTEX_ATTRIBUTE_FLOAT2:
            *out_gl_type = GL_FLOAT;
            *out_count = 2;
            break;
        case VERTEX_ATTRIBUTE_FLOAT3:
            *out_gl_type = GL_FLOAT;
            *out_count = 3;
            break;
        case VERTEX_ATTRIBUTE_FLOAT4:
            *out_gl_type = GL_FLOAT;
            *out_count = 4;
            break;
        case VERTEX_ATTRIBUTE_INT:
            *out_gl_type = GL_INT;
            *out_count = 1;
            break;
        case VERTEX_ATTRIBUTE_INT2:
            *out_gl_type = GL_INT;
            *out_count = 2;
            break;
        case VERTEX_ATTRIBUTE_INT3:
            *out_gl_type = GL_INT;
            *out_count = 3;
            break;
        case VERTEX_ATTRIBUTE_INT4:
            *out_gl_type = GL_INT;
            *out_count = 4;
            break;
        default:
            *out_gl_type = GL_FLOAT;
            *out_count = 1;
            break;
    }
}

b8 buffer_opengl_create(Buffer* buffer, const BufferConfig* config) {
    if (!buffer || !config) {
        return false;
    }

    OpenGLBufferState* buffer_state = vara_allocate(sizeof(OpenGLBufferState));
    vara_zero_memory(buffer_state, sizeof(OpenGLBufferState));
    if (!buffer_state) {
        return false;
    }

    buffer_state->target = type_to_gl_type(config->type);

    glGenBuffers(1, &buffer_state->id);
    if (buffer_state->id == 0) {
        vara_free(buffer_state, sizeof(OpenGLBufferState));
        return false;
    }

    glBindBuffer(buffer_state->target, buffer_state->id);
    glBufferData(
        buffer_state->target,
        (GLsizeiptr)config->size,
        config->data,
        usage_to_gl_usage(config->usage)
    );

    if (config->type == BUFFER_TYPE_UNIFORM) {
        glBindBufferBase(buffer_state->target, config->binding, buffer_state->id);
    }

    if (config->type == BUFFER_TYPE_VERTEX && config->layout) {
        glGenVertexArrays(1, &buffer_state->vao);
        glBindVertexArray(buffer_state->vao);

        glBindBuffer(GL_ARRAY_BUFFER, buffer_state->id);

        const VertexLayout* layout = config->layout;
        for (u32 i = 0; i < layout->attribute_count; i++) {
            const VertexAttribute* attribute = &layout->attributes[i];

            GLenum gl_type;
            GLint component_count;
            attribute_to_gl_attribute(attribute->type, &gl_type, &component_count);

            glEnableVertexAttribArray(attribute->location);
            glVertexAttribPointer(
                attribute->location,
                component_count,
                gl_type,
                attribute->normalized ? GL_TRUE : GL_FALSE,
                (GLsizei)layout->stride,
                (void*)(u64)attribute->offset
            );
        }

        glBindVertexArray(0);
    }

    glBindBuffer(buffer_state->target, 0);

    buffer->backend_data = buffer_state;
    return true;
}

void buffer_opengl_destroy(Buffer* buffer) {
    if (!buffer || !buffer->backend_data) {
        return;
    }

    OpenGLBufferState* buffer_state = buffer->backend_data;

    if (buffer_state->vao != 0) {
        glDeleteVertexArrays(1, &buffer_state->vao);
        buffer_state->vao = 0;
    }

    if (buffer_state->id != 0) {
        glDeleteBuffers(1, &buffer_state->id);
        buffer_state->id = 0;
    }

    vara_free(buffer_state, sizeof(OpenGLBufferState));
    buffer->backend_data = NULL;
}

void buffer_opengl_bind(Buffer* buffer) {
    if (!buffer || !buffer->backend_data) {
        return;
    }

    OpenGLBufferState* buffer_state = buffer->backend_data;

    if (buffer->type == BUFFER_TYPE_VERTEX && buffer_state->vao != 0) {
        glBindVertexArray(buffer_state->vao);
    } else {
        glBindBuffer(buffer_state->target, buffer_state->id);
    }
}

void buffer_opengl_unbind(Buffer* buffer) {
    if (!buffer || !buffer->backend_data) {
        return;
    }

    OpenGLBufferState* buffer_state = buffer->backend_data;

    if (buffer->type == BUFFER_TYPE_VERTEX && buffer_state->vao != 0) {
        glBindVertexArray(0);
    } else {
        glBindBuffer(buffer_state->target, 0);
    }
}

void buffer_opengl_set_data(Buffer* buffer, const void* data, size_t size, size_t offset) {
    if (!buffer || !buffer->backend_data) {
        return;
    }
    OpenGLBufferState* state = buffer->backend_data;

    glBindBuffer(state->target, state->id);
    glBufferSubData(state->target, (GLsizeiptr)offset, (GLsizeiptr)size, data);
    glBindBuffer(state->target, 0);
}
