#include <stdio.h>
#include <stdlib.h>

#include <core/core.h>
#include <core/defines.h>
#include <core/logger.h>
#include <core/math/math.h>
#include <core/platform/platform.h>
#include <core/platform/platform_window.h>

u64 sleep_time = 5;

i32 main(void) {
    DEBUG("Version: %s", VARA_VERSION);

    const vara_window_config window_config = {
        .position_x = 100,
        .position_y = 100,
        .width = 800,
        .height = 600,
        .title = "Vara Engine - Sandbox",
        .name = "vara_sandbox"
    };

    vara_window window = {0};

    if (!platform_window_create(&window, &window_config)) {
        ERROR("Failed to create window!");
        return EXIT_FAILURE;
    }

    INFO("Window created successfully: %dx%d", window.width, window.height);

    b8 running = true;
    while (running) {
        running = platform_window_poll_events();

        platform_sleep(16);
    }

    INFO("Shutting down...");
    platform_window_destroy(&window);

    return EXIT_SUCCESS;
}
