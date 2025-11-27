#include "vara/application/application.h"

#include <vara/core/event/event.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

typedef struct ApplicationState ApplicationState;

struct ApplicationState {
    ApplicationConfig config;
    VaraWindow* window;
    f64 last_time;
    b8 is_running;
};

static ApplicationState application_state;

int application_main(int argc, char** argv) {
    if (!platform_create()) {
        ERROR("Failed to create platform!");
        return EXIT_FAILURE;
    }
    event_system_create();

    application_init(&application_state.config);
    logging_system_create(application_state.config.level);
    input_system_create();

    INFO("Running Vara Engine (%s)", VARA_VERSION);

    if (application_state.config.window_config) {
        application_state.window =
            platform_window_create(application_state.config.window_config);
        if (!application_state.window) {
            ERROR("Failed to create window!");
            platform_destroy();
            return EXIT_FAILURE;
        }
    }

    if (application_state.config.renderer_type != RENDERER_TYPE_NONE) {
        if (!renderer_create(application_state.window)) {
            ERROR("Failed to create renderer!");
            platform_window_destroy(application_state.window);
            platform_destroy();
            return EXIT_FAILURE;
        }
    }

    if (application_state.config.app.on_init) {
        if (application_state.window) {
            platform_window_set_visible(application_state.window, true);
        }
        application_state.config.app.on_init();
    }

    application_state.is_running = true;
    application_state.last_time = (f32)platform_get_time();

    while (application_state.is_running) {
        if (application_state.window) {
            if (platform_window_should_close(application_state.window)) {
                application_state.is_running = false;
                break;
            }
            platform_poll_events();
        }

        const f64 current_time = platform_get_time();
        const f32 delta_time =
            (f32)(current_time - application_state.last_time);
        application_state.last_time = current_time;

        input_system_update();
        if (application_state.config.app.on_update) {
            application_state.config.app.on_update(delta_time);
        }
    }

    if (application_state.config.app.on_shutdown) {
        application_state.config.app.on_shutdown();
    }

    if (application_state.window) {
        platform_poll_events();
        platform_window_destroy(application_state.window);
    }

    if (renderer_get_instance()) {
        renderer_destroy();
    }

    input_system_destroy();
    logging_system_destroy();
    event_system_destroy();
    platform_destroy();

    return EXIT_SUCCESS;
}

VaraWindow* application_get_window(void) {
    return application_state.window;
}

void application_exit(void) {
    event_fire(EVENT_APPLICATION_QUIT, NULL, NULL);
    application_state.is_running = false;
}
