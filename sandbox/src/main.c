#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/math/types.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_window.h>

i32 application_main(void) {
    DEBUG("Version: %s", VARA_VERSION);
    if (!platform_create()) {
        ERROR("Failed to create platform!");
        return EXIT_FAILURE;
    }

    const VaraWindowConfig window_config = {
        .position_x = 100,
        .position_y = 100,
        .width = 800,
        .height = 600,
        .title = "Vara Engine - Sandbox",
        .name = "vara_sandbox"
    };

    VaraWindow* window = platform_window_create(&window_config);
    DEBUG("Making context current.");
    platform_window_make_context_current(window);

    DEBUG("Loading OpenGL");
    i32 version = gladLoadGL((GLADloadfunc)platform_window_get_proc_address);
    DEBUG(
        "Loaded OpenGL: %d.%d | %s | %s",
        GLAD_VERSION_MAJOR(version),
        GLAD_VERSION_MINOR(version),
        glGetString(GL_VENDOR),
        glGetString(GL_RENDERER)
    );

    while (!platform_window_should_close(window)) {
        platform_poll_events();

        // Set title to window dimensions.
        Vector2i dimensions = platform_window_get_size(window);
        char title_message[4096];
        platform_zero_memory(title_message, sizeof(title_message));
        snprintf(
            title_message,
            sizeof(title_message),
            "%s | %dx%d",
            window_config.title,
            dimensions.x,
            dimensions.y
        );
        platform_window_set_title(window, title_message);
    }

    INFO("Shutting down...");
    platform_window_destroy(window);
    platform_destroy();

    return EXIT_SUCCESS;
}
