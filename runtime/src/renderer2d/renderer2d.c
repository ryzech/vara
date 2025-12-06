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
#include "vara/shaders/renderer2d_text.glsl.gen.h"

static Renderer2D* renderer;

static Texture* default_texture;
static Shader* sprite_shader;
static Shader* text_shader;

static void renderer2d_flush(void) {
    if (renderer->vertex_count == 0) {
        return;
    }

    buffer_set_data(
        renderer->vertex_buffer, renderer->vertices, sizeof(Vertex) * renderer->vertex_count, 0
    );
    buffer_set_data(
        renderer->index_buffer, renderer->indices, sizeof(u32) * renderer->index_count, 0
    );

    int samplers[16];
    for (i32 i = 0; i < renderer->texture_count; i++) {
        texture_bind(renderer->textures[i], i);
        samplers[i] = i;
    }

    RenderCommandBuffer* command = renderer_get_frame_command_buffer();

    render_cmd_shader_set_int_array(
        command, sprite_shader, "uTextures", samplers, renderer->texture_count
    );
    render_cmd_draw_indexed(
        command, sprite_shader, renderer->vertex_buffer, renderer->index_buffer
    );
    renderer->draw_calls++;

    renderer->vertex_count = 0;
    renderer->index_count = 0;
    renderer->texture_count = 1;
    renderer->textures[0] = renderer->textures[0];
}

static void renderer2d_add_quad(
    Vector2 pos, Vector2 size, Vector2 uv0, Vector2 uv1, Texture* texture, Vector4 color
) {
    if (!renderer || !texture) {
        return;
    }

    f32 tex_index = 0;
    b8 found = false;
    for (u32 i = 0; i < renderer->texture_count; i++) {
        if (renderer->textures[i] == texture) {
            tex_index = (f32)i;
            found = true;
            break;
        }
    }

    if (!found) {
        if (renderer->texture_count >= RENDERER2D_MAX_TEXTURES) {
            renderer2d_flush();
            tex_index = 0;
        } else {
            tex_index = (f32)renderer->texture_count;
            renderer->textures[renderer->texture_count++] = texture;
        }
    }

    u32 current_vertex_count = renderer->vertex_count;

    const Vertex v0 = {
        .position = vec2_to_vec3(pos),
        .color = vec4_to_vec3(color),
        .tex_coord = {uv0.x, uv0.y},
        .tex_index = tex_index,
    };
    const Vertex v1 = {
        .position = {pos.x + size.x, pos.y, 0.0f},
        .color = vec4_to_vec3(color),
        .tex_coord = {uv1.x, uv0.y},
        .tex_index = tex_index,
    };
    const Vertex v2 = {
        .position = {pos.x + size.x, pos.y + size.y, 0.0f},
        .color = vec4_to_vec3(color),
        .tex_coord = {uv1.x, uv1.y},
        .tex_index = tex_index,
    };
    const Vertex v3 = {
        .position = {pos.x, pos.y + size.y, 0.0f},
        .color = vec4_to_vec3(color),
        .tex_coord = {uv0.x, uv1.y},
        .tex_index = tex_index,
    };

    renderer->vertices[renderer->vertex_count + 0] = v0;
    renderer->vertices[renderer->vertex_count + 1] = v1;
    renderer->vertices[renderer->vertex_count + 2] = v2;
    renderer->vertices[renderer->vertex_count + 3] = v3;
    renderer->vertex_count += 4;

    renderer->indices[renderer->index_count + 0] = current_vertex_count + 0;
    renderer->indices[renderer->index_count + 1] = current_vertex_count + 1;
    renderer->indices[renderer->index_count + 2] = current_vertex_count + 2;
    renderer->indices[renderer->index_count + 3] = current_vertex_count + 2;
    renderer->indices[renderer->index_count + 4] = current_vertex_count + 3;
    renderer->indices[renderer->index_count + 5] = current_vertex_count + 0;
    renderer->index_count += 6;
}

b8 renderer2d_create(const Renderer2DConfig* config) {
    renderer = platform_allocate(sizeof(Renderer2D));
    platform_zero_memory(renderer, sizeof(Renderer2D));

    if (!renderer) {
        return false;
    }

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
            .type = VERTEX_ATTRIBUTE_FLOAT3,
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
            .type = VERTEX_ATTRIBUTE_FLOAT,
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
    renderer->vertex_buffer = buffer_create(&vertex_buffer_config);

    const BufferConfig index_buffer_config = {
        .type = BUFFER_TYPE_INDEX,
        .usage = BUFFER_USAGE_DYNAMIC,
        .data = NULL,
        .size = sizeof(u32) * renderer->max_indices,
    };
    renderer->index_buffer = buffer_create(&index_buffer_config);

    ShaderSource sprite_sources[] = {
        {.stage = SHADER_STAGE_VERTEX, .source = renderer2d_sprite_vertex_source},
        {.stage = SHADER_STAGE_FRAGMENT, .source = renderer2d_sprite_fragment_source},
    };
    const ShaderConfig sprite_shader_config = {
        .name = "renderer2d_sprite_shader",
        .stages = sprite_sources,
        .stage_count = 2,
    };
    sprite_shader = shader_create(&sprite_shader_config);

    ShaderSource text_sources[] = {
        {.stage = SHADER_STAGE_VERTEX, .source = renderer2d_text_vertex_source},
        {.stage = SHADER_STAGE_FRAGMENT, .source = renderer2d_text_fragment_source},
    };
    const ShaderConfig text_shader_config = {
        .name = "renderer2d_text_shader",
        .stages = text_sources,
        .stage_count = 2,
    };
    text_shader = shader_create(&text_shader_config);

    // Set the default white texture if no texture is requested.
    u32 white_pixels = 0xffffffff;
    TextureConfig default_texture_config = {
        .width = 1,
        .height = 1,
        .format = TEXTURE_FORMAT_RGBA,
        .filter = TEXTURE_FILTER_LINEAR,
    };
    default_texture = texture_create(&default_texture_config);
    texture_set_data(default_texture, &white_pixels, sizeof(u32));

    return true;
}

void renderer2d_destroy(void) {
    if (renderer) {
        buffer_destroy(renderer->vertex_buffer);
        buffer_destroy(renderer->index_buffer);
        shader_destroy(sprite_shader);
        shader_destroy(text_shader);
        texture_destroy(default_texture);

        platform_free(renderer->vertices);
        platform_free(renderer->indices);
        platform_free(renderer);
    }
}

void renderer2d_begin(void) {
    VaraWindow* window = application_get_window();
    const Vector2i size = platform_window_get_size(window);
    const Matrix4 ortho = mat4_ortho(0.0f, (f32)size.x, (f32)size.y, 0.0f, -1.0f, 1.0f);

    RenderCommandBuffer* command = renderer_get_frame_command_buffer();
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

void renderer2d_draw_rect(Rect rect, Vector4 color) {
    renderer2d_draw_sprite(rect, default_texture, color);
}

void renderer2d_draw_sprite(Rect rect, Texture* texture, Vector4 tint) {
    renderer2d_add_quad(
        rect.position,
        rect.size,
        (Vector2){
            0,
            0,
        },
        (Vector2){
            1,
            1,
        },
        texture,
        tint
    );
}

void renderer2d_draw_text(const char* text, struct Font* font, Vector2 position, Vector4 color) {
    if (!renderer || !font || !text) {
        return;
    }

    f32 x = position.x;
    const f32 y = position.y;

    const char* ptr = text;
    while (*ptr) {
        const char c = *ptr;
        ptr++;

        if (c < 32 || c > 126) {
            continue;
        }

        const Glyph* glyph = &font->glyphs[c - 32];

        const Vector2 glyph_pos = {
            x + glyph->bearing.x,
            y + glyph->bearing.y,
        };
        const Vector2 glyph_size = glyph->size;

        renderer2d_add_quad(
            glyph_pos, glyph_size, glyph->uv_top_left, glyph->uv_bottom_right, font->atlas, color
        );

        x += glyph->advance;
    }
}
