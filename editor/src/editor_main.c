#include <vara/application/application.h>
#include <vara/core/defines.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/render_pass.h>
#include <vara/renderer2d/renderer2d.h>

static RenderPass* ui_pass;

void editor_init(void) {
    const RenderPassConfig ui_pass_config = {
        .name = "ui_pass",
        .target = NULL,
        .clear = true,
    };
    ui_pass = render_pass_create(&ui_pass_config);
}

void editor_update(f32 delta_time) {
    if (input_is_key_down(KEY_ESCAPE)) {
        application_exit();
    }

    render_pass_begin(ui_pass);

    const Rect rect = {
        .position = {100.0f, 100.0f},
        .size = {200.0f, 200.0f},
    };
    Vector4 color = {0.1f, 0.5f, 0.4f, 1.0f};
    if (point_in_rect(input_get_mouse_position(), rect)) {
        color.y = 0.6f;
    }
    renderer2d_draw_rect(rect, color);

    render_pass_end(ui_pass);
}

void editor_shutdown(void) {
    INFO("Shutting down...");
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
    config->level = LOG_LEVEL_DEBUG;
    config->window_config = &window_config;
    config->renderer_type = window_config.renderer_type;

    config->app.on_init = editor_init;
    config->app.on_update = editor_update;
    config->app.on_shutdown = editor_shutdown;
}
