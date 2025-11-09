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
    RendererInstance* instance;
    switch (config.graphics_type) {
        case GRAPHICS_TYPE_OPENGL:
            instance = renderer_opengl_init(window);
            platform_window_make_context_current(window);
            break;
        default:
            ERROR(
                "There is no graphics API of type('%s') supported!",
                graphics_type_to_string(config.graphics_type)
            );
            return EXIT_FAILURE;
    }

    renderer_create(instance);
    platform_window_set_visible(window, true);

    Vector4 clear_color = {.x = 0.0, .y = 0.5, .z = 0.5, .w = 1.0};

    while (!platform_window_should_close(window)) {
        platform_poll_events();
        renderer_clear_color(instance, clear_color);
        renderer_clear(instance);

        // TODO: add sin to internal math header.
        clear_color.x = 0.5 + 0.5 * (sinf(platform_get_time()));

        renderer_present(instance);
    }

    INFO("Shutting down...");
    renderer_destroy(instance);
    platform_window_destroy(window);
    platform_poll_events();
    platform_destroy();

    return EXIT_SUCCESS;
}
