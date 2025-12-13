#include <vara/application/application.h>
#include <vara/core/defines.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/render_pass.h>
#include <vara/scene/component.h>
#include <vara/scene/node.h>
#include <vara/scene/scene.h>

static RenderPass* ui_pass;
static Scene scene;
static Node root;

void editor_init(void) {
    const RenderPassConfig ui_pass_config = {
        .name = "ui_pass",
        .target = NULL,
        .clear = true,
    };
    ui_pass = render_pass_create(&ui_pass_config);

    scene = scene_create();
    root = node_create(scene, "Root");

    const SpriteComponent square_sprite = {
        .color = {0.5f, 0.5f, 0.5f, 1.0f},
        .z_index = 0,
    };
    const TransformComponent square_transform = {
        .translation = {100.0f, 100.0f, 0.0f},
        .scale = {200.0f, 200.0f, 1.0f},
    };
    const Node colored_square = node_create(scene, "Square");
    node_set_component(colored_square, TransformComponent, &square_transform);
    node_set_component(colored_square, SpriteComponent, &square_sprite);

    node_add_child(root, colored_square);
}

void editor_update(f32 delta_time) {
    if (input_is_key_down(KEY_ESCAPE)) {
        application_exit();
    }

    render_pass_begin(ui_pass);
    scene_update(scene, delta_time);
    render_pass_end(ui_pass);
}

void editor_shutdown(void) {
    INFO("Shutting down...");
    node_destroy(root);
    scene_destroy(scene);
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
