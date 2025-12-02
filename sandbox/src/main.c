#include <stdio.h>
#include <vara/application/application.h>
#include <vara/camera/camera.h>
#include <vara/core/defines.h>
#include <vara/core/event/event.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/buffer.h>
#include <vara/renderer/render_pass.h>
#include <vara/renderer/shader.h>

#include "vara/renderer/render_command.h"

const char* vertex_src = "#version 330 core\n"
                         "layout (location = 0) in vec3 aPos;\n"
                         "layout (location = 1) in vec3 aColor;\n"
                         "uniform mat4 uTransform;\n"
                         "out vec3 vPosition;\n"
                         "out vec3 vColor;\n"
                         "void main() {\n"
                         "    vPosition = aPos;\n"
                         "    vColor = aColor;\n"
                         "    gl_Position = uTransform * vec4(aPos, 1.0);\n"
                         "}\n";

const char* fragment_src = "#version 330 core\n"
                           "layout(location = 0) out vec4 color;\n"
                           "in vec3 vPosition;\n"
                           "in vec3 vColor;\n"
                           "void main() {\n"
                           "    color = vec4(0.2, 0.3, 0.4, 1.0);\n"
                           "}\n";

static Buffer* index_buffer;
static Buffer* vertex_buffer;
static Shader* shader;
static RenderPass* render_pass;

static Camera* camera;

static f32 timer;

static b8 on_window_resize(i16 event_code, void* sender, const EventData* event) {
    const i32 width = event->i32[0];
    const i32 height = event->i32[1];
    const Vector2i size = {width, height};

    camera_update(camera, size);
    return false;
}

void sandbox_init(void) {
    event_register(EVENT_WINDOW_RESIZE, on_window_resize);

    Vertex vertices[] = {
        {.position = vec3(0.0f, 0.5f, 0.0f)},
        {.position = vec3(-0.5f, -0.5f, 0.0f)},
        {.position = vec3(0.5f, -0.5f, 0.0f)},
    };
    u32 indices[] = {0, 1, 2};

    VertexAttribute attributes[] = {
        {.location = 0, .type = VERTEX_ATTRIBUTE_FLOAT3, .offset = 0, .normalized = false},
    };
    VertexLayout layout = {
        .attributes = attributes, .attribute_count = 1, .stride = sizeof(Vertex)
    };

    const BufferConfig vertex_buffer_config = {
        .type = BUFFER_TYPE_VERTEX,
        .usage = BUFFER_USAGE_STATIC,
        .layout = &layout,
        .data = vertices,
        .size = sizeof(vertices)
    };
    vertex_buffer = buffer_create(&vertex_buffer_config);

    const BufferConfig index_buffer_config = {
        .type = BUFFER_TYPE_INDEX,
        .usage = BUFFER_USAGE_STATIC,
        .data = indices,
        .size = sizeof(indices)
    };
    index_buffer = buffer_create(&index_buffer_config);

    ShaderSource sources[] = {
        {
            .stage = SHADER_STAGE_VERTEX,
            .source = vertex_src,
        },
        {
            .stage = SHADER_STAGE_FRAGMENT,
            .source = fragment_src,
        },
    };

    const ShaderConfig shader_config = {
        .name = "basic_shader",
        .stages = sources,
        .stage_count = 2,
    };
    shader = shader_create(&shader_config);

    const RenderPassConfig pass_config = {
        .name = "main_pass",
        .target = NULL,
        .clear = true,
        .clear_color = (Vector4){0.1f, 0.1f, 0.1f, 1.0f},
    };
    render_pass = render_pass_create(&pass_config);

    camera = camera_create();
    camera_update(
        camera,
        (Vector2i){
            application_get_window()->width,
            application_get_window()->height,
        }
    );
}

void sandbox_update(f32 delta_time) {
    if (input_is_key_down(KEY_ESCAPE)) {
        application_exit();
    }

    timer += delta_time;
    // Otherwise title goes brrrr
    if (timer >= 0.5f) {
        // Temporary FPS debug title.
        f32 fps = 1.0f / delta_time;
        char fps_str[32];
        snprintf(fps_str, sizeof(fps_str), "Sandbox - %.2f fps", fps);
        platform_window_set_title(application_get_window(), fps_str);
        timer = 0.0f;
    }

    Vector3 delta = vec3_zero();
    const f32 speed = 5.0f * delta_time;

    // Lateral movements
    if (input_is_key_down(KEY_W) || input_is_key_down(KEY_UP)) {
        delta.z -= speed;
    }
    if (input_is_key_down(KEY_S) || input_is_key_down(KEY_DOWN)) {
        delta.z += speed;
    }
    if (input_is_key_down(KEY_A) || input_is_key_down(KEY_LEFT)) {
        delta.x -= speed;
    }
    if (input_is_key_down(KEY_D) || input_is_key_down(KEY_RIGHT)) {
        delta.x += speed;
    }

    // Vertical movements
    if (input_is_key_down(KEY_SPACE)) {
        delta.y += speed;
    }
    if (input_is_key_down(KEY_RIGHT_SHIFT) || input_is_key_down(KEY_LEFT_SHIFT)) {
        delta.y -= speed;
    }

    // Only move if delta isn't zero
    if (delta.x != 0.0f || delta.z != 0.0f || delta.y != 0.0f) {
        camera_move(camera, delta);
    }

    RenderCommandBuffer* buffer = renderer_get_frame_command_buffer();

    render_pass_begin(render_pass);
    {
        render_cmd_shader_set_mat4(
            buffer, shader, "uTransform", camera_get_view_projection(camera)
        );
        render_cmd_draw_indexed(buffer, shader, vertex_buffer, index_buffer);
    }
    render_pass_end(render_pass);
}

void sandbox_shutdown() {
    shader_destroy(shader);
    buffer_destroy(vertex_buffer);
    buffer_destroy(index_buffer);
    render_pass_destroy(render_pass);
}

void application_init(ApplicationConfig* config) {
    static VaraWindowConfig window_config = {
        .x = 100,
        .y = 100,
        .width = 800,
        .height = 600,
        .renderer_type = RENDERER_TYPE_OPENGL,
        .title = "Vara Engine - Sandbox",
        .name = "vara_sandbox"
    };

    config->name = window_config.name;
    config->level = LOG_LEVEL_DEBUG;
    config->window_config = &window_config;
    config->renderer_type = window_config.renderer_type;

    config->app.on_init = sandbox_init;
    config->app.on_update = sandbox_update;
    config->app.on_shutdown = sandbox_shutdown;
}
