#include <stdio.h>
#include <stdlib.h>

#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
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
        .title = "Vara Engine - Editor",
        .name = "vara_editor"
    };

    VaraWindow* window = platform_window_create(&window_config);
    platform_window_set_visible(window, true);

    while (!platform_window_should_close(window)) {
        platform_poll_events();
    }

    INFO("Shutting down...");
    platform_window_destroy(window);
    platform_destroy();

    return EXIT_SUCCESS;
}
