#include <vara/core/event/event.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/application/application.h"
#include "vara/renderer/internal/renderer_internal.h"

typedef struct ApplicationState ApplicationState;

struct ApplicationState {
    ApplicationConfig config;
    VaraWindow* window;
    f64 last_time;
    b8 is_running;
    Renderer* renderer;
};

static ApplicationState application_state;

static b8 application_on_window_resize(i16 event_code, void* sender, const EventData* event) {
    const i32 width = event->i32[0];
    const i32 height = event->i32[1];
    const Vector2i size = {width, height};

    if (application_state.renderer) {
        renderer_on_window_resize(application_state.renderer, size);
    }

    return false;
}

int application_main(int argc, char** argv) {
    if (!platform_create()) {
        ERROR("Failed to create platform!");
        return EXIT_FAILURE;
    }
    event_system_create();
    event_register(EVENT_WINDOW_RESIZE, application_on_window_resize);

    application_init(&application_state.config);
    logging_system_create(application_state.config.level);
    input_system_create();

    INFO("Running Vara Engine (%s)", VARA_VERSION);

    if (application_state.config.window_config) {
        application_state.window = platform_window_create(application_state.config.window_config);
        platform_window_make_context_current(application_state.window);
        if (!application_state.window) {
            ERROR("Failed to create window!");
            platform_destroy();
            return EXIT_FAILURE;
        }
    }

    if (application_state.config.renderer_type != RENDERER_TYPE_NONE) {
        application_state.renderer = renderer_create(application_state.window);
        if (!application_state.renderer) {
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
        const f32 delta_time = (f32)(current_time - application_state.last_time);
        application_state.last_time = current_time;

        input_system_update();
        if (application_state.config.app.on_update) {
            if (application_state.renderer) {
                renderer_begin_frame(application_state.renderer);
            }
            application_state.config.app.on_update(delta_time);
            if (application_state.renderer) {
                renderer_end_frame(application_state.renderer);
            }
        }
    }

    if (application_state.config.app.on_shutdown) {
        application_state.config.app.on_shutdown();
    }

    if (application_state.window) {
        platform_poll_events();
        platform_window_destroy(application_state.window);
    }

    if (application_state.renderer) {
        renderer_destroy(application_state.renderer);
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

Renderer* application_get_renderer(void) {
    return application_state.renderer;
}

void application_exit(void) {
    event_fire(EVENT_APPLICATION_QUIT, NULL, NULL);
    application_state.is_running = false;
}
