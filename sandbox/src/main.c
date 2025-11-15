#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/buffer.h>
#include <vara/renderer/render_pass.h>
#include <vara/renderer/renderer.h>
#include <vara/renderer/shader.h>

const char* vertex_src = "#version 330 core\n"
                         "layout (location = 0) in vec3 aPos;\n"
                         "void main() {\n"
                         "    gl_Position = vec4(aPos, 1.0);\n"
                         "}\n";

const char* fragment_src = "#version 330 core\n"
                           "out vec4 FragColor;\n"
                           "void main() {\n"
                           "    FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
                           "}\n";

i32 application_main(void) {
    DEBUG("Version: %s", VARA_VERSION);
    if (!platform_create()) {
        ERROR("Failed to create platform!");
        return EXIT_FAILURE;
    }

    const VaraConfig config = {
        .graphics_type = GRAPHICS_TYPE_OPENGL,
        .application_name = "vara_sandbox"
    };

    const VaraWindowConfig window_config = {
        .position_x = 100,
        .position_y = 100,
        .width = 800,
        .height = 600,
        .graphics_type = config.graphics_type,
        .title = "Vara Engine - Sandbox",
        .name = config.application_name
    };

    VaraWindow* window = platform_window_create(&window_config);
    if (!window) {
        return EXIT_FAILURE;
    }

    RendererInstance* instance = renderer_create(window);
    if (!instance) {
        return EXIT_FAILURE;
    }

    f32 vertices[] = {
        0.0f,
        0.5f,
        0.0f,
        -0.5f,
        -0.5f,
        0.0f,
        0.5f,
        -0.5f,
        0.0f
    };

    u32 indices[] = {
        0, 1, 2
    };

    VertexAttribute attributes[] = {
        {.location = 0,
         .type = VERTEX_ATTRIBUTE_FLOAT3,
         .offset = 0,
         .normalized = false}
    };

    VertexLayout layout = {
        .attributes = attributes,
        .attribute_count = 1,
        .stride = 3 * sizeof(f32)
    };

    const BufferConfig vertex_buffer_config = {
        .type = BUFFER_TYPE_VERTEX,
        .usage = BUFFER_USAGE_STATIC,
        .layout = &layout,
        .data = vertices,
        .size = sizeof(vertices)
    };

    Buffer* vertex_buffer = buffer_create(instance, &vertex_buffer_config);

    const BufferConfig index_buffer_config = {
        .type = BUFFER_TYPE_INDEX,
        .usage = BUFFER_USAGE_STATIC,
        .data = indices,
        .size = sizeof(indices)
    };

    Buffer* index_buffer = buffer_create(instance, &index_buffer_config);

    ShaderSource sources[] = {
        {
            .stage = SHADER_STAGE_VERTEX,
            .source = vertex_src,
        },
        {
            .stage = SHADER_STAGE_FRAGMENT,
            .source = fragment_src,
        },
    };

    const ShaderConfig shader_config = {
        .name = "basic_shader",
        .stages = sources,
        .stage_count = 2,
    };

    Shader* shader = shader_create(instance, &shader_config);

    const RenderPassConfig pass_config = {
        .name = "main_pass",
    };

    RenderPass* pass = render_pass_create(instance, &pass_config);

    platform_window_set_visible(window, true);

    while (!platform_window_should_close(window)) {
        platform_poll_events();
        renderer_clear(instance);

        render_pass_begin(pass);

        shader_bind(shader);

        render_pass_draw_indexed(pass, vertex_buffer, index_buffer);

        shader_unbind(shader);

        render_pass_end(pass);

        renderer_present(instance);
    }

    INFO("Shutting down...");
    shader_destroy(shader);
    renderer_destroy(instance);
    platform_window_destroy(window);
    platform_poll_events();
    platform_destroy();

    return EXIT_SUCCESS;
}
