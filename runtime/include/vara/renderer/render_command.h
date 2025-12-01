#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

typedef enum RenderCommandType RenderCommandType;
typedef struct RenderCommandHeader RenderCommandHeader;
typedef struct RenderCommandBuffer RenderCommandBuffer;

struct Buffer;
struct Shader;
struct RenderPass;

enum RenderCommandType {
    RENDER_CMD_BEGIN_PASS,
    RENDER_CMD_DRAW,
    RENDER_CMD_DRAW_INDEXED,
    RENDER_CMD_SET_SHADER_MAT4,
    RENDER_CMD_SET_SHADER_INT_ARRAY,
    RENDER_CMD_END_PASS,
};

struct RenderCommandHeader {
    RenderCommandType type;
    u32 size;
};

struct RenderCommandBuffer {
    u8* buffer;
    u32 used;
    u32 capacity;
};

RenderCommandBuffer* render_cmd_buffer_create(void);
void render_cmd_buffer_destroy(RenderCommandBuffer* buffer);
void render_cmd_buffer_reset(RenderCommandBuffer* buffer);

void render_cmd_begin_pass(RenderCommandBuffer* buffer, struct RenderPass* pass);
void render_cmd_shader_set_mat4(
    RenderCommandBuffer* buffer, struct Shader* shader, const char* name, Matrix4 matrix
);
void render_cmd_shader_set_int_array(
    RenderCommandBuffer* buffer,
    struct Shader* shader,
    const char* name,
    const i32* array,
    u32 count
);
void render_cmd_draw_indexed(
    RenderCommandBuffer* buffer,
    struct RenderPass* pass,
    struct Shader* shader,
    struct Buffer* vertex,
    struct Buffer* index
);
void render_cmd_end_pass(RenderCommandBuffer* buffer, struct RenderPass* pass);
void render_cmd_execute(RenderCommandBuffer* buffer);
