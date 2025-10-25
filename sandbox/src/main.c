#include <stdio.h>
#include <stdlib.h>

#include <core/defines.h>
#include <core/logger.h>
#include <core/math/math.h>
#include <core/platform/platform.h>
#include <core/platform/platform_window.h>
#include "core/math/types.h"

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
