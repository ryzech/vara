#include <vara/core/platform/platform.h>

#include "vara/application/application.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer2d/renderer2d.h"

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
    };
    VertexLayout layout = {
        .attributes = attributes,
        .attribute_count = 2,
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

    const char* vertex_src = "#version 330 core\n"
                             "layout(location = 0) in vec3 aPos;\n"
                             "layout(location = 1) in vec3 aColor;\n"
                             "uniform mat4 uTransform;\n"
                             "out vec3 vColor;\n"
                             "void main() {\n"
                             "    vColor = aColor;\n"
                             "    gl_Position = uTransform * vec4(aPos, 1.0);\n"
                             "}\n";

    const char* fragment_src = "#version 330 core\n"
                               "in vec3 vColor;\n"
                               "out vec4 FragColor;\n"
                               "void main() {\n"
                               "    FragColor = vec4(vColor, 1.0);\n"
                               "}\n";

    ShaderSource sources[] = {
        {.stage = SHADER_STAGE_VERTEX, .source = vertex_src},
        {.stage = SHADER_STAGE_FRAGMENT, .source = fragment_src},
    };
    const ShaderConfig shader_config = {
        .name = "renderer2d_shader",
        .stages = sources,
        .stage_count = 2,
    };
    renderer->shader = shader_create(&shader_config);

    renderer->render_pass = config->render_pass;
    ;

    return renderer;
}

void renderer2d_destroy(Renderer2D* renderer) {
    if (renderer) {
        buffer_destroy(renderer->vertex_buffer);
        buffer_destroy(renderer->index_buffer);
        shader_destroy(renderer->shader);

        platform_free(renderer->vertices);
        platform_free(renderer->indices);
        platform_free(renderer);
    }
}

void renderer2d_begin(Renderer2D* renderer) {
    renderer->vertex_count = 0;
    renderer->index_count = 0;
}

void renderer2d_end(Renderer2D* renderer) {
    if (renderer->vertex_count == 0) {
        return;
    }

    INFO("renderer2d_end: Drawing %d vertices, %d indices",
         renderer->vertex_count, renderer->index_count);
    buffer_bind(renderer->vertex_buffer);
    buffer_set_data(
        renderer->vertex_buffer, renderer->vertices, sizeof(Vertex) * renderer->vertex_count, 0
    );

    buffer_bind(renderer->index_buffer);
    buffer_set_data(
        renderer->index_buffer, renderer->indices, sizeof(u32) * renderer->index_count, 0
    );

    RenderCommandBuffer* command = renderer_get_frame_command_buffer();
    VaraWindow* window = application_get_window();
    Vector2i size = platform_window_get_size(window);
    Matrix4 ortho = mat4_ortho(0.0f, (f32)size.x, (f32)size.y, 0.0f, -1.0f, 1.0f);

    render_cmd_begin_pass(command, renderer->render_pass);
    render_cmd_shader_set_mat4(command, renderer->shader, "uTransform", ortho);
    render_cmd_draw_indexed(
        command,
        renderer->render_pass,
        renderer->shader,
        renderer->vertex_buffer,
        renderer->index_buffer
    );
    render_cmd_end_pass(command, renderer->render_pass);
}

void renderer2d_draw_rect(Renderer2D* renderer, Rect rect, Vector4 color) {
    if (!renderer) {
        return;
    }

    u32 current_vertex_count = renderer->vertex_count;

    const Vertex v0 = {.position = rect.position, .color = vec4_to_vec3(color)};
    const Vertex v1 = {
        .position =
            {
                rect.position.x + rect.size.x,
                rect.position.y,
                rect.position.z,
            },
        .color = vec4_to_vec3(color)
    };
    const Vertex v2 = {
        .position =
            {
                rect.position.x + rect.size.x,
                rect.position.y + rect.size.y,
                rect.position.z,
            },
        .color = vec4_to_vec3(color)
    };
    const Vertex v3 = {
        .position =
            {
                rect.position.x,
                rect.position.y + rect.size.y,
                rect.position.z,
            },
        .color = vec4_to_vec3(color)
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
