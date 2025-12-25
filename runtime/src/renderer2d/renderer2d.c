#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>

#include "vara/application/application.h"
#include "vara/renderer/buffer.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer/shader.h"
#include "vara/renderer/texture.h"
#include "vara/renderer2d/renderer2d.h"

#include "vara/renderer/font.h"
#include "vara/shaders/renderer2d_sprite.glsl.gen.h"

static Renderer2D* renderer;

static Texture* default_texture;
static Shader* sprite_shader;

static i32 compare_draw_commands(const void* a, const void* b) {
    const DrawCommand* cmd_a = a;
    const DrawCommand* cmd_b = b;

    if (cmd_a->z_index != cmd_b->z_index) {
        return cmd_b->z_index - cmd_a->z_index;
    }

    return (i32)cmd_b->submission_id - (i32)cmd_a->submission_id;
}

static void renderer2d_render_batch(void) {
    if (renderer->vertex_count == 0) {
        return;
    }

    buffer_set_data(
        renderer->vertex_buffer, renderer->vertices, sizeof(Vertex) * renderer->vertex_count, 0
    );
    buffer_set_data(
        renderer->index_buffer, renderer->indices, sizeof(u32) * renderer->index_count, 0
    );

    for (i32 i = 0; i < renderer->texture_count; i++) {
        texture_bind(renderer->textures[i], i);
    }

    RenderCommandBuffer* command = renderer_get_frame_command_buffer(renderer->renderer);
    render_cmd_draw_indexed(
        command, sprite_shader, renderer->vertex_buffer, renderer->index_buffer
    );
    renderer->draw_calls++;

    renderer->vertex_count = 0;
    renderer->index_count = 0;
    renderer->texture_count = 1;
    renderer->textures[0] = renderer->textures[0];
}

static void renderer2d_flush(void) {
    if (renderer->command_count == 0) {
        return;
    }

    qsort(renderer->commands, renderer->command_count, sizeof(DrawCommand), compare_draw_commands);

    for (u32 c = 0; c < renderer->command_count; c++) {
        const DrawCommand* cmd = &renderer->commands[c];

        b8 batch_full = false;

        f32 tex_slot = 0;
        b8 found = false;

        for (int i = 0; i < renderer->texture_count; i++) {
            if (renderer->textures[i] == cmd->texture) {
                tex_slot = (f32)i;
                found = true;
                break;
            }
        }

        if (!found && renderer->texture_count >= RENDERER2D_MAX_TEXTURES) {
            batch_full = true;
        }

        if (renderer->vertex_count + cmd->vertex_count > renderer->max_vertices
            || renderer->index_count + cmd->index_count > renderer->max_indices) {
            batch_full = true;
        }

        if (batch_full) {
            renderer2d_render_batch();
        }

        if (!found) {
            tex_slot = (f32)renderer->texture_count;
            renderer->textures[renderer->texture_count++] = cmd->texture;
        }

        const u32 current_batch_vertex_count = renderer->vertex_count;

        for (u32 i = 0; i < cmd->vertex_count; i++) {
            Vertex vert = cmd->vertices[i];
            vert.tex_index = tex_slot;
            renderer->vertices[renderer->vertex_count++] = vert;
        }

        for (u32 i = 0; i < cmd->index_count; i++) {
            renderer->indices[renderer->index_count++] =
                cmd->indices[i] + current_batch_vertex_count;
        }
    }

    renderer2d_render_batch();

    renderer->command_count = 0;
    renderer->submission_count = 0;
}

static void renderer2d_submit_command(
    Shader* shader,
    Texture* texture,
    Vertex* vertices,
    u32 vertex_count,
    u32* indices,
    u32 index_count,
    i32 z_index
) {
    if (renderer->command_count >= RENDERER2D_MAX_COMMANDS) {
        renderer2d_flush();

        if (renderer->command_count >= RENDERER2D_MAX_COMMANDS) {
            WARN("Command buffer overflow after flush. Dropping draw call.");
            return;
        }
    }

    DrawCommand* cmd = &renderer->commands[renderer->command_count];
    platform_copy_memory(cmd->vertices, vertices, vertex_count * sizeof(Vertex));
    platform_copy_memory(cmd->indices, indices, index_count * sizeof(u32));

    cmd->shader = shader;
    cmd->texture = texture;
    cmd->vertex_count = vertex_count;
    cmd->index_count = index_count;
    cmd->z_index = z_index;
    cmd->submission_id = renderer->submission_count++;

    renderer->command_count++;
}

b8 renderer2d_create(Renderer* backend, const Renderer2DConfig* config) {
    renderer = platform_allocate(sizeof(Renderer2D));
    platform_zero_memory(renderer, sizeof(Renderer2D));

    if (!renderer) {
        return false;
    }

    renderer->renderer = backend;
    renderer->max_vertices = config->max_vertices;
    renderer->max_indices = config->max_indices;

    renderer->vertices = platform_allocate(renderer->max_vertices * sizeof(Vertex));
    renderer->indices = platform_allocate(renderer->max_indices * sizeof(u32));

    VertexAttribute attributes[] = {
        {
            .location = 0,
            .type = VERTEX_ATTRIBUTE_FLOAT3,
            .offset = 0,
            .normalized = false,
        },
        {
            .location = 1,
            .type = VERTEX_ATTRIBUTE_FLOAT4,
            .offset = offsetof(Vertex, color),
            .normalized = false,
        },
        {
            .location = 2,
            .type = VERTEX_ATTRIBUTE_FLOAT2,
            .offset = offsetof(Vertex, tex_coord),
            .normalized = false,
        },
        {
            .location = 3,
            .type = VERTEX_ATTRIBUTE_INT,
            .offset = offsetof(Vertex, tex_index),
            .normalized = false,
        },
    };
    VertexLayout layout = {
        .attributes = attributes,
        .attribute_count = 4,
        .stride = sizeof(Vertex),
    };

    const BufferConfig vertex_buffer_config = {
        .type = BUFFER_TYPE_VERTEX,
        .usage = BUFFER_USAGE_DYNAMIC,
        .layout = &layout,
        .data = NULL,
        .size = sizeof(Vertex) * renderer->max_vertices,
    };
    renderer->vertex_buffer = buffer_create(renderer->renderer, &vertex_buffer_config);

    const BufferConfig index_buffer_config = {
        .type = BUFFER_TYPE_INDEX,
        .usage = BUFFER_USAGE_DYNAMIC,
        .data = NULL,
        .size = sizeof(u32) * renderer->max_indices,
    };
    renderer->index_buffer = buffer_create(renderer->renderer, &index_buffer_config);

    ShaderSource sprite_sources[] = {
        {.stage = SHADER_STAGE_VERTEX, .source = renderer2d_sprite_vertex_source},
        {.stage = SHADER_STAGE_FRAGMENT, .source = renderer2d_sprite_fragment_source},
    };
    const ShaderConfig sprite_shader_config = {
        .name = "renderer2d_sprite_shader",
        .stages = sprite_sources,
        .stage_count = 2,
    };
    sprite_shader = shader_create(renderer->renderer, &sprite_shader_config);

    // Set the default white texture if no texture is requested.
    u32 white_pixels = 0xffffffff;
    TextureConfig default_texture_config = {
        .width = 1,
        .height = 1,
        .format = TEXTURE_FORMAT_RGBA,
        .filter = TEXTURE_FILTER_LINEAR,
    };
    default_texture = texture_create(renderer->renderer, &default_texture_config);
    texture_set_data(default_texture, &white_pixels, sizeof(u32));

    int samplers[RENDERER2D_MAX_TEXTURES];
    for (i32 i = 0; i < renderer->texture_count; i++) {
        samplers[i] = i;
    }
    RenderCommandBuffer* command = renderer_get_frame_command_buffer(renderer->renderer);
    render_cmd_shader_set_int_array(
        command, sprite_shader, "uTextures", samplers, renderer->texture_count
    );

    return true;
}

void renderer2d_destroy(void) {
    if (renderer) {
        buffer_destroy(renderer->vertex_buffer);
        buffer_destroy(renderer->index_buffer);
        shader_destroy(sprite_shader);
        texture_destroy(default_texture);

        platform_free(renderer->vertices);
        platform_free(renderer->indices);
        platform_free(renderer);
    }
}

void renderer2d_begin(Renderer* backend) {
    VaraWindow* window = application_get_window();
    const Vector2i size = platform_window_get_size(window);
    const Matrix4 ortho = mat4_ortho(0.0f, (f32)size.x, (f32)size.y, 0.0f, -1.0f, 1.0f);

    renderer->renderer = backend;
    RenderCommandBuffer* command = renderer_get_frame_command_buffer(renderer->renderer);
    render_cmd_shader_set_mat4(command, sprite_shader, "uProjection", ortho);

    renderer->draw_calls = 0;
    renderer->vertex_count = 0;
    renderer->index_count = 0;
    renderer->texture_count = 1;
    renderer->textures[0] = default_texture;
}

void renderer2d_end() {
    renderer2d_flush();
}

void renderer2d_draw_rect(const Vector2 position, const Vector2 size, Vector4 color, i32 z_index) {
    const Matrix4 transform = mat4_mul(
        mat4_translation(vec2_to_vec3(position, 0.0f)), mat4_scale(vec2_to_vec3(size, 1.0f))
    );
    renderer2d_draw_rect_matrix(transform, color, z_index);
}

void renderer2d_draw_rect_matrix(const Matrix4 matrix, Vector4 color, i32 z_index) {
    Geometry2D quad = geometry_generate_quad(vec2_one(), vec2_zero(), vec2_one(), color);
    geometry_transform(&quad, &matrix);

    renderer2d_submit_command(
        sprite_shader,
        default_texture,
        quad.vertices,
        quad.vertex_count,
        quad.indices,
        quad.index_count,
        z_index
    );

    geometry_destroy(&quad);
}

void renderer2d_draw_sprite(
    const Vector2 position, const Vector2 size, Texture* texture, Vector4 tint, i32 z_index
) {
    const Matrix4 transform = mat4_mul(
        mat4_translation(vec2_to_vec3(position, 0.0f)), mat4_scale(vec2_to_vec3(size, 1.0f))
    );
    renderer2d_draw_sprite_matrix(transform, texture, tint, z_index);
}

void renderer2d_draw_sprite_matrix(
    const Matrix4 matrix, Texture* texture, Vector4 tint, i32 z_index
) {
    Geometry2D quad = geometry_generate_quad(vec2_one(), vec2_zero(), vec2_one(), tint);
    geometry_transform(&quad, &matrix);

    renderer2d_submit_command(
        sprite_shader,
        texture,
        quad.vertices,
        quad.vertex_count,
        quad.indices,
        quad.index_count,
        z_index
    );

    geometry_destroy(&quad);
}

// TODO: figure out how to get the size auto-calculated, while letting you change it.
void renderer2d_draw_text(
    const Vector2 position,
    const Vector2 size,
    const char* text,
    struct Font* font,
    Vector4 color,
    i32 z_index
) {
    const Matrix4 transform = mat4_mul(
        mat4_translation(vec2_to_vec3(position, 0.0f)), mat4_scale(vec2_to_vec3(size, 1.0f))
    );
    renderer2d_draw_text_matrix(transform, text, font, color, z_index);
}

void renderer2d_draw_text_matrix(
    const Matrix4 matrix, const char* text, struct Font* font, Vector4 color, i32 z_index
) {
    if (!renderer || !font || !text) {
        return;
    }

    f32 x = 0.0f;
    const f32 y = 0.0f;

    const char* ptr = text;
    while (*ptr) {
        const char c = *ptr;
        ptr++;

        if (c < 32 || c > 126) {
            continue;
        }

        const Glyph* glyph = &font->glyphs[c - 32];

        const Rect rect = {
            .position = {x + glyph->bearing.x, y + glyph->bearing.y},
            .size = glyph->size,
        };

        Geometry2D quad =
            geometry_generate_quad(rect.size, glyph->uv_top_left, glyph->uv_bottom_right, color);
        geometry_transform(&quad, &matrix);

        renderer2d_submit_command(
            sprite_shader,
            font->atlas,
            quad.vertices,
            quad.vertex_count,
            quad.indices,
            quad.index_count,
            z_index
        );

        geometry_destroy(&quad);

        x += glyph->advance;
    }
}
