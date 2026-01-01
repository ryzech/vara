#include <stdio.h>
#include <vara/application/application.h>
#include <vara/camera/camera.h>
#include <vara/core/event/event.h>
#include <vara/core/input/input.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>
#include <vara/material/material.h>
#include <vara/renderer/buffer.h>
#include <vara/renderer/framebuffer.h>
#include <vara/renderer/render_packet.h>
#include <vara/renderer/render_pass.h>
#include <vara/renderer/shader.h>
#include <vara/renderer/texture.h>
#include <vara/shaders/basic_shader.glsl.gen.h>
#include <vara/shaders/screen_quad.glsl.gen.h>

static Buffer* index_buffer;
static Buffer* vertex_buffer;
static Shader* shader;
static Shader* screen_shader;
static RenderPass* render_pass;
static RenderPass* screen_pass;
static Framebuffer* render_buffer;

static Camera* camera;

static f32 timer;

static b8 on_window_resize(i16 event_code, void* sender, const EventData* event) {
    const i32 width = event->i32[0];
    const i32 height = event->i32[1];
    const Vector2i size = {width, height};

    // Needs the physical size in pixels rather than logical.
    framebuffer_resize(render_buffer, width * 2, height * 2);

    camera_update(camera, size);
    return false;
}

void sandbox_init(void) {
    event_register(EVENT_WINDOW_RESIZE, on_window_resize);

    Renderer* renderer = application_get_renderer();
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
    vertex_buffer = buffer_create(renderer, &vertex_buffer_config);

    const BufferConfig index_buffer_config = {
        .type = BUFFER_TYPE_INDEX,
        .usage = BUFFER_USAGE_STATIC,
        .data = indices,
        .size = sizeof(indices)
    };
    index_buffer = buffer_create(renderer, &index_buffer_config);

    ShaderSource sources[] = {
        {
            .stage = SHADER_STAGE_VERTEX,
            .source = basic_shader_vertex_source,
        },
        {
            .stage = SHADER_STAGE_FRAGMENT,
            .source = basic_shader_fragment_source,
        },
    };
    const ShaderConfig shader_config = {
        .name = "basic_shader",
        .stages = sources,
        .stage_count = 2,
    };
    shader = shader_create(renderer, &shader_config);

    ShaderSource screen_sources[] = {
        {
            .stage = SHADER_STAGE_VERTEX,
            .source = screen_quad_vertex_source,
        },
        {
            .stage = SHADER_STAGE_FRAGMENT,
            .source = screen_quad_fragment_source,
        },
    };
    const ShaderConfig screen_shader_config = {
        .name = "screen_shader",
        .stages = screen_sources,
        .stage_count = 2,
    };
    screen_shader = shader_create(renderer, &screen_shader_config);

    const Vector2i size = platform_window_get_size(application_get_window());
    FramebufferAttachmentConfig attachments[] = {
        {.type = FRAMEBUFFER_ATTACHMENT_COLOR, .format = FRAMEBUFFER_FORMAT_RGBA8},
        {.type = FRAMEBUFFER_ATTACHMENT_DEPTH, .format = FRAMEBUFFER_FORMAT_DEPTH24_STENCIL8},
    };
    const FramebufferConfig fb_config = {
        .name = "offscreen_fb",
        .attachments = attachments,
        .attachment_count = 2,
        .width = size.x * 2,
        .height = size.y * 2,
        .samples = 1,
    };
    render_buffer = framebuffer_create(renderer, &fb_config);

    const RenderPassConfig pass_config = {
        .name = "main_pass",
        .target = render_buffer,
        .clear = true,
        .clear_color = (Vector4){0.1f, 0.1f, 0.1f, 1.0f},
    };
    render_pass = render_pass_create(renderer, &pass_config);

    const RenderPassConfig screen_pass_config = {
        .name = "screen_pass",
        .target = NULL,
        .clear = true,
        .clear_color = vec4(0.2f, 0.2f, 0.2f, 1.0f),
    };
    screen_pass = render_pass_create(renderer, &screen_pass_config);

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

    Renderer* renderer = application_get_renderer();
    render_pass_begin(render_pass);
    {
        Material material = {
            .shader = shader,
            .model = mat4_identity(),
            .view_projection = camera_get_view_projection(camera),
        };
        RenderPacket packet = {
            .material = &material,
            .vertex_buffer = vertex_buffer,
            .index_buffer = index_buffer,
            .index_count = 3,
        };
        render_pass_submit(render_pass, &packet);
    }
    render_pass_end(renderer, render_pass);

    render_pass_begin(screen_pass);
    {
        Texture* screen_texture = framebuffer_get_attachment(render_buffer, 0);
        Material material = {
            .shader = screen_shader,
            .model = mat4_identity(),
            .view_projection = mat4_identity(),
            .texture_count = 1,
        };
        material.textures[0] = screen_texture;
        RenderPacket packet = {
            .material = &material,
            .vertex_count = 3,
        };
        render_pass_submit(screen_pass, &packet);
    }
    render_pass_end(renderer, screen_pass);
}

void sandbox_shutdown() {
    framebuffer_destroy(render_buffer);
    shader_destroy(shader);
    shader_destroy(screen_shader);
    buffer_destroy(vertex_buffer);
    buffer_destroy(index_buffer);
    render_pass_destroy(render_pass);
    render_pass_destroy(screen_pass);
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
