#include <vara/application/application.h>
#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform_window.h>

void editor_init(void) {
    DEBUG("Version: %s", VARA_VERSION);
}

void editor_update(f32 delta_time) {

}

void editor_shutdown(void) {
    INFO("Shutting down...");
}

void application_init(ApplicationConfig* config) {
    static VaraWindowConfig window_config = {
        .position_x = 100,
        .position_y = 100,
        .width = 800,
        .height = 600,
        .graphics_type = GRAPHICS_TYPE_OPENGL,
        .title = "Vara Engine - Editor",
        .name = "vara_editor"
    };

    config->name = window_config.name;
    config->level = LOG_LEVEL_DEBUG;
    config->window_config = &window_config;
    config->graphics_type = window_config.graphics_type;

    config->app.on_init = editor_init;
    config->app.on_update = editor_update;
    config->app.on_shutdown = editor_shutdown;
}
