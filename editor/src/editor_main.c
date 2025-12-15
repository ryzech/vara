#include <vara/application/application.h>
#include <vara/core/defines.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/renderer/render_pass.h>

#include "editor/editor_panel.h"
#include "editor/editor_ui.h"

static RenderPass* ui_pass;

void editor_init(void) {
    const RenderPassConfig ui_pass_config = {
        .name = "ui_pass",
        .target = NULL,
        .clear = true,
    };
    ui_pass = render_pass_create(&ui_pass_config);

    editor_ui_create();
}

void editor_update(f32 delta_time) {
    if (input_is_key_down(KEY_ESCAPE)) {
        application_exit();
    }

    editor_ui_update(delta_time);
    render_pass_begin(ui_pass);

    editor_ui_draw();

    render_pass_end(ui_pass);
}

void editor_shutdown(void) {
    INFO("Shutting down...");
    editor_ui_destroy();
    render_pass_destroy(ui_pass);
}

void application_init(ApplicationConfig* config) {
    static VaraWindowConfig window_config = {
        .x = 100,
        .y = 100,
        .width = 800,
        .height = 600,
        .renderer_type = RENDERER_TYPE_OPENGL,
        .title = "Vara Engine - Editor",
        .name = "vara_editor"
    };

    config->name = window_config.name;
    config->level = LOG_LEVEL_TRACE;
    config->window_config = &window_config;
    config->renderer_type = window_config.renderer_type;

    config->app.on_init = editor_init;
    config->app.on_update = editor_update;
    config->app.on_shutdown = editor_shutdown;
}
