#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/material/material.h"
#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/render_packet.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/renderer.h"

static void render_pass_build_commands(RenderPass* pass) {
    RenderCommandBuffer* buffer = pass->command_buffer;
    for (u32 i = 0; i < pass->packet_count; i++) {
        const RenderPacket* packet = &pass->packets[i];
        const Material* material = packet->material;

        render_cmd_bind_shader(buffer, material->shader);

        for (u32 t = 0; t < material->texture_count; t++) {
            render_cmd_bind_texture(buffer, material->textures[t], t);
        }

        render_cmd_shader_set_mat4(
            buffer, material->shader, "uViewProj", material->view_projection
        );
        render_cmd_shader_set_mat4(buffer, material->shader, "uModel", material->model);

        // Should we allow non-indexed drawing?
        if (packet->vertex_buffer && packet->index_buffer && packet->index_count > 0) {
            render_cmd_bind_buffer(buffer, packet->vertex_buffer);
            render_cmd_bind_buffer(buffer, packet->index_buffer);
            render_cmd_draw_indexed(buffer, packet->index_count, packet->first_index);
        }
    }
}

RenderPass* render_pass_create(Renderer* renderer, const RenderPassConfig* config) {
    RenderPass* pass = platform_allocate(sizeof(RenderPass));
    platform_zero_memory(pass, sizeof(RenderPass));

    pass->name = config->name;
    pass->target = config->target;
    pass->clear = config->clear;
    pass->clear_color = config->clear_color;

    RendererBackend* backend = renderer_backend_get(renderer);
    pass->backend = backend;

    pass->packet_capacity = 256;
    pass->packets = platform_allocate(sizeof(RenderPacket) * pass->packet_capacity);
    pass->command_buffer = render_cmd_buffer_create();

    if (!pass->backend->render_pass.create(pass, config)) {
        ERROR("Failed to create render pass named('%s')", config->name);
        render_pass_destroy(pass);
        return NULL;
    }

    return pass;
}

void render_pass_destroy(RenderPass* pass) {
    if (pass) {
        pass->backend->render_pass.destroy(pass);
        render_cmd_buffer_destroy(pass->command_buffer);
        platform_free(pass->packets);
        platform_free(pass);
    }
}

void render_pass_begin(RenderPass* pass) {
    pass->packet_count = 0;
    render_cmd_buffer_reset(pass->command_buffer);
    render_cmd_begin_pass(pass->command_buffer, pass);
}

void render_pass_end(Renderer* renderer, RenderPass* pass) {
    // TODO: sort commands
    render_pass_build_commands(pass);
    render_cmd_end_pass(pass->command_buffer, pass);
    renderer_execute_commands(renderer, pass->command_buffer);
}

void render_pass_submit(RenderPass* pass, RenderPacket* packet) {
    if (pass->packet_count >= pass->packet_capacity) {
        pass->packet_capacity *= 2;
        void* resized_array = platform_allocate(sizeof(RenderPacket) * pass->packet_capacity);
        platform_copy_memory(
            resized_array, pass->packets, sizeof(RenderPacket) * pass->packet_count
        );
        platform_free(pass->packets);
        pass->packets = resized_array;
    }
    pass->packets[pass->packet_count++] = *packet;
}
