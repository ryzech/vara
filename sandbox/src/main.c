#include <stdio.h>
#include <stdlib.h>
#include <vara/application/application.h>
#include <vara/camera/camera.h>
#include <vara/core/defines.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_graphics_types.h>
#include <vara/core/platform/platform_window.h>
#include <vara/renderer/renderer.h>

const char* vertex_src = "#version 330 core\n"
                         "layout (location = 0) in vec3 aPos;\n"
                         "uniform mat4 uTransform;\n"
                         "out vec3 vPosition;\n"
                         "void main() {\n"
                         "    vPosition = aPos;\n"
                         "    gl_Position = uTransform * vec4(aPos, 1.0);\n"
                         "}\n";

const char* fragment_src = "#version 330 core\n"
                           "layout(location = 0) out vec4 color;\n"
                           "in vec3 vPosition;\n"
                           "void main() {\n"
                           "    color = vec4(vPosition * 0.5 + 0.5, 1.0);\n"
                           "}\n";

static Buffer* index_buffer;
static Buffer* vertex_buffer;
static Shader* shader;
static RenderPass* render_pass;

static Camera* camera;

void sandbox_init(void) {
    DEBUG("Version: %s", VARA_VERSION);
    f32 vertices[] = {0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f};

    u32 indices[] = {0, 1, 2};

    VertexAttribute attributes[] = {
        {.location = 0,
         .type = VERTEX_ATTRIBUTE_FLOAT3,
         .offset = 0,
         .normalized = false}
    };

    VertexLayout layout = {
        .attributes = attributes,
        .attribute_count = 1,
        .stride = 3 * sizeof(f32)
    };

    const BufferConfig vertex_buffer_config = {
        .type = BUFFER_TYPE_VERTEX,
        .usage = BUFFER_USAGE_STATIC,
        .layout = &layout,
        .data = vertices,
        .size = sizeof(vertices)
    };

    vertex_buffer =
        buffer_create(application_get_renderer(), &vertex_buffer_config);

    const BufferConfig index_buffer_config = {
        .type = BUFFER_TYPE_INDEX,
        .usage = BUFFER_USAGE_STATIC,
        .data = indices,
        .size = sizeof(indices)
    };

    index_buffer =
        buffer_create(application_get_renderer(), &index_buffer_config);

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

    shader = shader_create(application_get_renderer(), &shader_config);

    const RenderPassConfig pass_config = {
        .name = "main_pass",
        .target = NULL,
    };

    render_pass = render_pass_create(application_get_renderer(), &pass_config);

    camera = camera_create();
    camera_set_position(
        camera,
        (Vector3){
            0.0f,
            0.0f,
            0.5f,
        }
    );
    camera->projection = mat4_perspective(
        degrees_to_radians(60.0f),
        (f32)application_get_window()->width
            / (f32)application_get_window()->height,
        0.01f,
        100.0f
    );
    camera_move(camera, vec3_zero());
}

void sandbox_update(f32 delta_time) {
    if (input_is_key_down(KEY_ESCAPE)) {
        application_exit();
    }

    Vector3 delta = vec3_zero();
    const f32 speed = 5.0f * delta_time;

    if (input_is_key_down(KEY_W)) {
        delta.z -= speed;
    }
    if (input_is_key_down(KEY_S)) {
        delta.z += speed;
    }
    if (input_is_key_down(KEY_A)) {
        delta.x -= speed;
    }
    if (input_is_key_down(KEY_D)) {
        delta.x += speed;
    }

    if (delta.x != 0.0f || delta.z != 0.0f) {
        camera_move(camera, delta);
    }

    const Matrix4 transform_matrix =
        mat4_mul(camera_get_projection(camera), camera_get_view(camera));

    renderer_clear_color(
        application_get_renderer(),
        (Vector4){
            0.1f,
            0.1f,
            0.1f,
            1.0f,
        }
    );
    renderer_clear(application_get_renderer());

    render_pass_begin(render_pass);
    {
        shader_set_mat4(shader, "uTransform", &transform_matrix);
        render_pass_draw_indexed(
            render_pass, shader, vertex_buffer, index_buffer
        );
    }
    render_pass_end(render_pass);

    renderer_present(application_get_renderer());
}

void sandbox_shutdown() {
    shader_destroy(shader);
    buffer_destroy(vertex_buffer);
    buffer_destroy(index_buffer);

    camera_destroy(camera);
    render_pass_destroy(render_pass);
}

void application_init(ApplicationConfig* config) {
    static VaraWindowConfig window_config = {
        .position_x = 100,
        .position_y = 100,
        .width = 800,
        .height = 600,
        .graphics_type = GRAPHICS_TYPE_OPENGL,
        .title = "Vara Engine - Sandbox",
        .name = "vara_sandbox"
    };

    config->name = window_config.name;
    config->level = LOG_LEVEL_DEBUG;
    config->window_config = &window_config;
    config->graphics_type = window_config.graphics_type;

    config->app.on_init = sandbox_init;
    config->app.on_update = sandbox_update;
    config->app.on_shutdown = sandbox_shutdown;
}
