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
                           "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
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

    ShaderSource sources[] = {
        {.stage = SHADER_STAGE_VERTEX, .source = vertex_src},
        {.stage = SHADER_STAGE_FRAGMENT, .source = fragment_src},
    };

    ShaderConfig shader_config = {
        .name = "basic_shader",
        .stages = sources,
        .stage_count = 2,
    };

    Shader* shader = shader_create(instance, &shader_config);

    platform_window_set_visible(window, true);

    while (!platform_window_should_close(window)) {
        platform_poll_events();
        renderer_clear(instance);

        shader_bind(shader);

        shader_unbind(shader);

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
