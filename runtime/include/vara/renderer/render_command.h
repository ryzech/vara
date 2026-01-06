#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

typedef enum RenderCommandType RenderCommandType;
typedef struct RenderCommandHeader RenderCommandHeader;
typedef struct RenderCommandBuffer RenderCommandBuffer;
typedef struct RenderCmdBeginPass RenderCmdBeginPass;
typedef struct RenderCmdEndPass RenderCmdEndPass;
typedef struct RenderCmdBindPipeline RenderCmdBindPipeline;
typedef struct RenderCmdBindShader RenderCmdBindShader;
typedef struct RenderCmdBindBuffer RenderCmdBindBuffer;
typedef struct RenderCmdBindTexture RenderCmdBindTexture;
typedef struct RenderCmdDraw RenderCmdDraw;
typedef struct RenderCmdDrawIndexed RenderCmdDrawIndexed;
typedef struct RenderCmdSetShaderMat4 RenderCmdSetShaderMat4;
typedef struct RenderCmdSetShaderIntArray RenderCmdSetShaderIntArray;

struct Buffer;
struct Shader;
struct Texture;
struct RenderPass;
struct RenderPipeline;

enum RenderCommandType {
    RENDER_CMD_BEGIN_PASS,
    RENDER_CMD_BIND_PIPELINE,
    RENDER_CMD_BIND_SHADER,
    RENDER_CMD_BIND_BUFFER,
    RENDER_CMD_BIND_TEXTURE,
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

struct RenderCmdBeginPass {
    RenderCommandHeader header;
    struct RenderPass* pass;
};

struct RenderCmdEndPass {
    RenderCommandHeader header;
    struct RenderPass* pass;
};

struct RenderCmdDraw {
    RenderCommandHeader header;
    u32 vertex_count;
    u32 first_vertex;
};

struct RenderCmdDrawIndexed {
    RenderCommandHeader header;
    u32 index_count;
    u32 first_index;
};

struct RenderCmdBindPipeline {
    RenderCommandHeader header;
    struct RenderPipeline* pipeline;
};

struct RenderCmdBindShader {
    RenderCommandHeader header;
    struct Shader* shader;
};

struct RenderCmdBindBuffer {
    RenderCommandHeader header;
    struct Buffer* buffer;
};

struct RenderCmdBindTexture {
    RenderCommandHeader header;
    struct Texture* texture;
    u32 slot;
};

struct RenderCmdSetShaderMat4 {
    RenderCommandHeader header;
    Matrix4 matrix;
    struct Shader* shader;
    const char* name;
};

struct RenderCmdSetShaderIntArray {
    RenderCommandHeader header;
    i32 array[32];
    struct Shader* shader;
    u32 count;
    const char* name;
};

RenderCommandBuffer* render_cmd_buffer_create(void);
void render_cmd_buffer_destroy(RenderCommandBuffer* buffer);
void render_cmd_buffer_reset(RenderCommandBuffer* buffer);

void render_cmd_begin_pass(RenderCommandBuffer* buffer, struct RenderPass* pass);
void render_cmd_end_pass(RenderCommandBuffer* buffer, struct RenderPass* pass);
void render_cmd_bind_pipeline(RenderCommandBuffer* buffer, struct RenderPipeline* pipeline);
void render_cmd_bind_shader(RenderCommandBuffer* buffer, struct Shader* shader);
void render_cmd_bind_buffer(RenderCommandBuffer* buffer, struct Buffer* input);
void render_cmd_bind_texture(RenderCommandBuffer* buffer, struct Texture* texture, u32 slot);
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
void render_cmd_draw(RenderCommandBuffer* buffer, u32 vertex_count, u32 first_vertex);
void render_cmd_draw_indexed(RenderCommandBuffer* buffer, u32 index_count, u32 first_index);