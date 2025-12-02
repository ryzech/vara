#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>

#include "vara/application/application.h"
#include "vara/renderer/buffer.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer/shader.h"
#include "vara/renderer/texture.h"
#include "vara/renderer2d/renderer2d.h"
#include "vara/shaders/renderer2d.glsl.gen.h"

static Texture* default_texture;

static void renderer2d_flush(Renderer2D* renderer) {
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
        command, renderer->shader, "uTextures", samplers, renderer->texture_count
    );
    render_cmd_draw_indexed(
        command, renderer->shader, renderer->vertex_buffer, renderer->index_buffer
    );

    renderer->vertex_count = 0;
    renderer->index_count = 0;
    renderer->texture_count = 1;
    renderer->textures[0] = renderer->textures[0];
}

Renderer2D* renderer2d_create(const Renderer2DConfig* config) {
    Renderer2D* renderer = platform_allocate(sizeof(Renderer2D));
    platform_zero_memory(renderer, sizeof(Renderer2D));

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

    ShaderSource sources[] = {
        {.stage = SHADER_STAGE_VERTEX, .source = renderer2d_vertex_source},
        {.stage = SHADER_STAGE_FRAGMENT, .source = renderer2d_fragment_source},
    };
    const ShaderConfig shader_config = {
        .name = "renderer2d_shader",
        .stages = sources,
        .stage_count = 2,
    };
    renderer->shader = shader_create(&shader_config);

    // Set the default white texture if no texture is requested.
    u8 white_pixels[4] = {255, 255, 255, 255};
    TextureConfig default_texture_config = {
        .width = 1,
        .height = 1,
        .format = TEXTURE_FORMAT_RGBA,
        .filter = TEXTURE_FILTER_LINEAR,
    };
    default_texture = texture_create(&default_texture_config);
    texture_set_data(default_texture, white_pixels, 4);

    return renderer;
}

void renderer2d_destroy(Renderer2D* renderer) {
    if (renderer) {
        buffer_destroy(renderer->vertex_buffer);
        buffer_destroy(renderer->index_buffer);
        shader_destroy(renderer->shader);
        texture_destroy(default_texture);

        platform_free(renderer->vertices);
        platform_free(renderer->indices);
        platform_free(renderer);
    }
}

void renderer2d_begin(Renderer2D* renderer) {
    VaraWindow* window = application_get_window();
    const Vector2i size = platform_window_get_size(window);
    const Matrix4 ortho = mat4_ortho(0.0f, (f32)size.x, (f32)size.y, 0.0f, -1.0f, 1.0f);

    RenderCommandBuffer* command = renderer_get_frame_command_buffer();
    render_cmd_shader_set_mat4(command, renderer->shader, "uProjection", ortho);

    renderer->vertex_count = 0;
    renderer->index_count = 0;
    renderer->texture_count = 1;
    renderer->textures[0] = default_texture;
}

void renderer2d_end(Renderer2D* renderer) {
    renderer2d_flush(renderer);
}

void renderer2d_draw_rect(Renderer2D* renderer, Rect rect, Vector4 color) {
    renderer2d_draw_rect_texture(renderer, rect, default_texture, color);
}

void renderer2d_draw_rect_texture(Renderer2D* renderer, Rect rect, Texture* texture, Vector4 tint) {
    if (!renderer || !texture) {
        return;
    }

    if (renderer->vertex_count + 4 > renderer->max_vertices
        || renderer->index_count + 6 > renderer->max_indices) {
        renderer2d_flush(renderer);
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
            renderer2d_flush(renderer);
            tex_index = 0;
        } else {
            tex_index = (f32)renderer->texture_count;
            renderer->textures[renderer->texture_count++] = texture;
        }
    }

    u32 current_vertex_count = renderer->vertex_count;

    const Vertex v0 = {
        .position = vec2_to_vec3(rect.position),
        .color = vec4_to_vec3(tint),
        .tex_coord = {0.0f, 0.0f},
        .tex_index = tex_index,
    };
    const Vertex v1 = {
        .position = {rect.position.x + rect.size.x, rect.position.y, 0.0f},
        .color = vec4_to_vec3(tint),
        .tex_coord = {1.0f, 0.0f},
        .tex_index = tex_index,
    };
    const Vertex v2 = {
        .position = {rect.position.x + rect.size.x, rect.position.y + rect.size.y, 0.0f},
        .color = vec4_to_vec3(tint),
        .tex_coord = {1.0f, 1.0f},
        .tex_index = tex_index,
    };
    const Vertex v3 = {
        .position = {rect.position.x, rect.position.y + rect.size.y, 0.0f},
        .color = vec4_to_vec3(tint),
        .tex_coord = {0.0f, 1.0f},
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
