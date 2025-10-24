#include <stdio.h>
#include <stdlib.h>

#include <core/core.h>
#include <core/defines.h>
#include <core/logger.h>
#include <core/math/math.h>
#include <core/platform/platform.h>
#include <core/platform/platform_window.h>

i32 main(void) {
    DEBUG("Version: %s", VARA_VERSION);
    if(!platform_create()) {
      ERROR("Failed to create platform!");
      return EXIT_FAILURE;
    }

    const platform_window_config window_config = {
        .position_x = 100,
        .position_y = 100,
        .width = 800,
        .height = 600,
        .title = "Vara Engine - Sandbox",
        .name = "vara_sandbox"
    };

    platform_window* window = platform_window_create(&window_config);


    while (!platform_window_should_close(window)) {
        platform_poll_events();
    }

    INFO("Shutting down...");
    platform_window_destroy(window);
    platform_destroy();

    return EXIT_SUCCESS;
}
