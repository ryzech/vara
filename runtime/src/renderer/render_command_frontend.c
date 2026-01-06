#include <vara/core/defines.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/buffer.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/render_pipeline.h"
#include "vara/renderer/shader.h"
#include "vara/renderer/texture.h"

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

void render_cmd_bind_pipeline(RenderCommandBuffer* buffer, RenderPipeline* pipeline) {
    RenderCmdBindPipeline* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdBindPipeline));
    cmd->header.type = RENDER_CMD_BIND_PIPELINE;
    cmd->header.size = sizeof(RenderCmdBindPipeline);
    cmd->pipeline = pipeline;
}

void render_cmd_bind_shader(RenderCommandBuffer* buffer, Shader* shader) {
    RenderCmdBindShader* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdBindShader));
    cmd->header.type = RENDER_CMD_BIND_SHADER;
    cmd->header.size = sizeof(RenderCmdBindShader);
    cmd->shader = shader;
}

void render_cmd_bind_buffer(RenderCommandBuffer* buffer, Buffer* input) {
    RenderCmdBindBuffer* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdBindBuffer));
    cmd->header.type = RENDER_CMD_BIND_BUFFER;
    cmd->header.size = sizeof(RenderCmdBindBuffer);
    cmd->buffer = input;
}

void render_cmd_bind_texture(RenderCommandBuffer* buffer, Texture* texture, u32 slot) {
    RenderCmdBindTexture* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdBindTexture));
    cmd->header.type = RENDER_CMD_BIND_TEXTURE;
    cmd->header.size = sizeof(RenderCmdBindTexture);
    cmd->texture = texture;
    cmd->slot = slot;
}

void render_cmd_draw(RenderCommandBuffer* buffer, u32 vertex_count, u32 first_vertex) {
    RenderCmdDraw* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdDraw));
    cmd->header.type = RENDER_CMD_DRAW;
    cmd->header.size = sizeof(RenderCmdDraw);
    cmd->vertex_count = vertex_count;
    cmd->first_vertex = first_vertex;
}

void render_cmd_draw_indexed(RenderCommandBuffer* buffer, u32 index_count, u32 first_index) {
    RenderCmdDrawIndexed* cmd = render_cmd_allocate(buffer, sizeof(RenderCmdDrawIndexed));
    cmd->header.type = RENDER_CMD_DRAW_INDEXED;
    cmd->header.size = sizeof(RenderCmdDrawIndexed);
    cmd->index_count = index_count;
    cmd->first_index = first_index;
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

void render_cmd_shader_set_int_array(
    RenderCommandBuffer* buffer, Shader* shader, const char* name, const i32* array, u32 count
) {
    RenderCmdSetShaderIntArray* cmd =
        render_cmd_allocate(buffer, sizeof(RenderCmdSetShaderIntArray));
    cmd->header.type = RENDER_CMD_SET_SHADER_INT_ARRAY;
    cmd->header.size = sizeof(RenderCmdSetShaderIntArray);
    cmd->shader = shader;
    cmd->name = name;
    cmd->count = count;

    platform_copy_memory(cmd->array, array, count * sizeof(i32));
}
