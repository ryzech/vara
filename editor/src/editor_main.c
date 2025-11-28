#include <vara/application/application.h>
#include <vara/core/defines.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer2d/renderer2d.h>

static Renderer2D* renderer;
static RenderPass* ui_pass;

void editor_init(void) {
    const RenderPassConfig ui_pass_config = {
        .name = "ui_pass",
        .target = NULL,
        .clear = true,
    };
    ui_pass = render_pass_create(&ui_pass_config);

    const Renderer2DConfig renderer2d_config = {
        .max_vertices = 32,
        .max_indices = 32,
        .render_pass = ui_pass,
    };
    renderer = renderer2d_create(&renderer2d_config);

    VaraWindow* window = application_get_window();
    Vector2i size = platform_window_get_size(window);
    renderer_set_viewport((Vector2i){0, 0}, size);
}

void editor_update(f32 delta_time) {
    if (input_is_key_down(KEY_ESCAPE)) {
        application_exit();
    }

    render_pass_begin(ui_pass);
    renderer2d_begin(renderer);

    const Rect rect = {
        .position = {100.0f, 100.0f, 0.0f},
        .size = {200.0f, 200.0f},
    };
    const Vector4 color = {0.8f, 0.2f, 0.2f, 1.0f};
    renderer2d_draw_rect(renderer, rect, color);

    renderer2d_end(renderer);
    render_pass_end(ui_pass);
}

void editor_shutdown(void) {
    INFO("Shutting down...");
    renderer2d_destroy(renderer);
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
