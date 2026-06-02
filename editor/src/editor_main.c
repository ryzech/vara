#include <vara/application/application.h>
#include <vara/core/defines.h>
#include <vara/core/event/event.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/renderer/buffer.h>
#include <vara/renderer/render_pass.h>
#include <vara/renderer/render_pipeline.h>
#include <vara/renderer/shader.h>
#include <vara/renderer/swapchain.h>
#include <vara/shaders/renderer2d_sprite.glsl.gen.h>

#include "editor/editor_panel.h"
#include "editor/editor_ui.h"

RenderPass* ui_pass = NULL;
RenderPipeline* ui_pipeline = NULL;
Buffer* ui_vertex_buffer = NULL;
Buffer* ui_index_buffer = NULL;
static Shader* ui_shader;

static b8 on_window_resize(i16 event_code, void* sender, const EventData* event) {
    const i32 width = event->i32[0];
    const i32 height = event->i32[1];
    const Vector2 size = {width, height};

    editor_ui_set_bounds(vec2_zero(), size);
    return false;
}

void editor_init(void) {
    event_register(EVENT_WINDOW_RESIZE, on_window_resize);

    Renderer* renderer = application_get_renderer();

    RenderPassAttachment screen_color = {
        .load = ATTACHMENT_LOAD_OP_CLEAR,
        .clear = vec4(0.2f, 0.2f, 0.2f, 1.0f),
    };
    const RenderPassConfig ui_pass_config = {
        .name = "ui_pass",
        .color_attachments = &screen_color,
        .color_attachment_count = 1,
    };
    ui_pass = render_pass_create(renderer, &ui_pass_config);

    ShaderSource sources[] = {
        {.stage = SHADER_STAGE_VERTEX, .source = renderer2d_sprite_vertex_source},
        {.stage = SHADER_STAGE_FRAGMENT, .source = renderer2d_sprite_fragment_source},
    };
    const ShaderConfig ui_shader_config = {
        .name = "editor_ui_shader",
        .stages = sources,
        .stage_count = 2,
    };
    ui_shader = shader_create(renderer, &ui_shader_config);

    const RenderPipelineConfig ui_pipeline_config = {
        .name = "editor_ui_pipeline",
        .shader = ui_shader,
        .pass = ui_pass,
    };
    ui_pipeline = render_pipeline_create(renderer, &ui_pipeline_config);

    const BufferConfig vb_config = {
        .type = BUFFER_TYPE_VERTEX,
        .usage = BUFFER_USAGE_DYNAMIC,
        .size = sizeof(Vertex) * 4 * 256,
    };
    ui_vertex_buffer = buffer_create(renderer, &vb_config);

    const BufferConfig ib_config = {
        .type = BUFFER_TYPE_INDEX,
        .usage = BUFFER_USAGE_DYNAMIC,
        .size = sizeof(u32) * 6 * 256,
    };
    ui_index_buffer = buffer_create(renderer, &ib_config);

    editor_ui_create(renderer);
}

void editor_update(f32 delta_time) {
    if (input_is_key_down(KEY_ESCAPE)) {
        application_exit();
    }

    Renderer* renderer = application_get_renderer();
    editor_ui_update(delta_time);
    render_pass_begin(ui_pass, swapchain_get_current_target(renderer->swapchain));
    {
        editor_ui_draw();
    }
    render_pass_end(renderer, ui_pass);
}

void editor_shutdown(void) {
    INFO("Shutting down...");
    editor_ui_destroy();
    render_pass_destroy(ui_pass);
    shader_destroy(ui_shader);
    render_pipeline_destroy(ui_pipeline);
    buffer_destroy(ui_vertex_buffer);
    buffer_destroy(ui_index_buffer);
}

void application_init(ApplicationConfig* config) {
    static VaraWindowConfig window_config = {
        .x = 100,
        .y = 100,
        .width = 800,
        .height = 600,
        .renderer_type = RENDERER_TYPE_VULKAN,
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
