#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"

typedef struct RenderCmdBeginPass RenderCmdBeginPass;
typedef struct RenderCmdEndPass RenderCmdEndPass;
typedef struct RenderCmdDrawIndexed RenderCmdDrawIndexed;
typedef struct RenderCmdSetShaderMat4 RenderCmdSetShaderMat4;
typedef struct RenderCmdClearColor RenderCmdClearColor;

struct RenderCmdBeginPass {
    RenderCommandHeader header;
    RenderPass* pass;
};

struct RenderCmdEndPass {
    RenderCommandHeader header;
    RenderPass* pass;
};

struct RenderCmdDrawIndexed {
    RenderCommandHeader header;
    RenderPass* pass;
    Shader* shader;
    Buffer* vertex;
    Buffer* index;
};

struct RenderCmdSetShaderMat4 {
    RenderCommandHeader header;
    Matrix4 matrix;
    Shader* shader;
    const char* name;
};

static void* render_cmd_allocate(RenderCommandBuffer* buffer, u32 size) {
    if (buffer->used + size > buffer->capacity) {
        // Increase buffer size.
        buffer->capacity *= 2;
        // Allocate new resized buffer.
        u8* resized_buffer = platform_allocate(buffer->capacity);
        // Copy old buffer.
        platform_copy_memory(resized_buffer, buffer->buffer, buffer->used);
        platform_free(buffer->buffer);
        buffer->buffer = resized_buffer;
    }

    void* cmd = buffer->buffer + buffer->used;
    buffer->used += size;
    return cmd;
}

RenderCommandBuffer* render_cmd_buffer_create(void) {
    RenderCommandBuffer* buffer = platform_allocate(sizeof(RenderCommandBuffer));
    buffer->capacity = 1024;
    buffer->used = 0;
    buffer->buffer = platform_allocate(buffer->capacity);
    return buffer;
}

void render_cmd_buffer_destroy(RenderCommandBuffer* buffer) {
    platform_free(buffer->buffer);
    platform_free(buffer);
}

void render_cmd_buffer_reset(RenderCommandBuffer* buffer) {
    buffer->used = 0;
}

void render_cmd_begin_pass(RenderCommandBuffer* buffer, RenderPass* pass) {
    RenderCmdBeginPass* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdBeginPass));
    cmd->header.type = RENDER_CMD_BEGIN_PASS;
    cmd->header.size = sizeof(RenderCmdBeginPass);
    cmd->pass = pass;
}

void render_cmd_end_pass(RenderCommandBuffer* buffer, RenderPass* pass) {
    RenderCmdEndPass* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdEndPass));
    cmd->header.type = RENDER_CMD_END_PASS;
    cmd->header.size = sizeof(RenderCmdEndPass);
    cmd->pass = pass;
}

void render_cmd_draw_indexed(
    RenderCommandBuffer* buffer,
    RenderPass* pass,
    struct Shader* shader,
    struct Buffer* vertex,
    struct Buffer* index
) {
    RenderCmdDrawIndexed* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdDrawIndexed));
    cmd->header.type = RENDER_CMD_DRAW_INDEXED;
    cmd->header.size = sizeof(RenderCmdDrawIndexed);
    cmd->pass = pass;
    cmd->shader = shader;
    cmd->vertex = vertex;
    cmd->index = index;
}

void render_cmd_shader_set_mat4(
    RenderCommandBuffer* buffer, Shader* shader, const char* name, Matrix4 matrix
) {
    RenderCmdSetShaderMat4* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdSetShaderMat4));
    cmd->header.type = RENDER_CMD_SET_SHADER_MAT4;
    cmd->header.size = sizeof(RenderCmdSetShaderMat4);
    cmd->shader = shader;
    cmd->name = name;
    cmd->matrix = matrix;
}

void render_cmd_execute(RenderCommandBuffer* buffer) {
    u8* cmd = buffer->buffer;
    const u8* end = buffer->buffer + buffer->used;

    while (cmd < end) {
        const RenderCommandHeader* header = (RenderCommandHeader*)cmd;

        switch (header->type) {
            case RENDER_CMD_BEGIN_PASS: {
                const RenderCmdBeginPass* begin_pass = (RenderCmdBeginPass*)cmd;
                begin_pass->pass->vt.render_pass_begin(begin_pass->pass);
                break;
            }
            case RENDER_CMD_END_PASS: {
                const RenderCmdEndPass* end_pass = (RenderCmdEndPass*)cmd;
                end_pass->pass->vt.render_pass_end(end_pass->pass);
                break;
            }
            case RENDER_CMD_DRAW_INDEXED: {
                const RenderCmdDrawIndexed* draw_indexed = (RenderCmdDrawIndexed*)cmd;
                shader_bind(draw_indexed->shader);
                buffer_bind(draw_indexed->vertex);
                buffer_bind(draw_indexed->index);
                draw_indexed->pass->vt.render_pass_draw_indexed(
                    draw_indexed->pass, draw_indexed->index
                );
                break;
            }
            case RENDER_CMD_SET_SHADER_MAT4: {
                const RenderCmdSetShaderMat4* set_mat4 = (RenderCmdSetShaderMat4*)cmd;
                shader_bind(set_mat4->shader);
                shader_set_mat4(set_mat4->shader, set_mat4->name, set_mat4->matrix);
                break;
            }
            default: {
                ERROR("Unknown render command type!");
                break;
            }
        }

        cmd += header->size;
    }
}
