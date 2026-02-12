#include <vara/core/logger.h>
#include <vara/core/memory/memory.h>

#include "vara/material/material.h"
#include "vara/renderer/internal/renderer_internal.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/render_packet.h"
#include "vara/renderer/render_pass.h"
#include "vara/renderer/renderer.h"

static b8 render_pass_validate_target(RenderPass* pass, RenderTarget* target) {
    if (!target) {
        ERROR("RenderPass named('%s') called with NULL RenderTarget", pass->name);
        return false;
    }

    return true;
}

static void render_pass_build_commands(RenderPass* pass) {
    RenderCommandBuffer* buffer = pass->command_buffer;
    for (u32 i = 0; i < pass->packet_count; i++) {
        const RenderPacket* packet = &pass->packets[i];
        const Material* material = packet->material;

        render_cmd_bind_pipeline(buffer, packet->pipeline);
        if (packet->material) {
            for (u32 t = 0; t < material->texture_count; t++) {
                render_cmd_bind_texture(buffer, material->textures[t], t);
            }
        }

        for (u32 j = 0; j < packet->uniform_buffer_count; j++) {
            render_cmd_bind_uniform_buffer(buffer, packet->pipeline, packet->uniform_buffers[j]);
        }

        // Should we allow non-indexed drawing?
        if (packet->vertex_buffer && packet->index_buffer && packet->index_count > 0) {
            render_cmd_bind_vertex_buffer(buffer, packet->vertex_buffer);
            render_cmd_bind_index_buffer(buffer, packet->index_buffer);
            render_cmd_draw_indexed(buffer, packet->index_count, packet->first_index);
        } else if (packet->vertex_count > 0) {
            if (packet->vertex_buffer) {
                render_cmd_bind_vertex_buffer(buffer, packet->vertex_buffer);
            }
            render_cmd_draw(buffer, packet->vertex_count, packet->first_vertex);
        }
    }
}

RenderPass* _render_pass_create(RendererBackend* backend, const RenderPassConfig* config) {
    RenderPass* pass = vara_allocate(sizeof(RenderPass));
    vara_zero_memory(pass, sizeof(RenderPass));

    pass->name = config->name;

    pass->color_attachment_count = config->color_attachment_count;
    if (config->color_attachment_count > 0) {
        pass->color_attachments =
            vara_allocate(sizeof(RenderPassAttachment) * config->color_attachment_count);
        vara_copy_memory(
            pass->color_attachments,
            config->color_attachments,
            sizeof(RenderPassAttachment) * config->color_attachment_count
        );
    }

    if (config->depth_stencil_attachment) {
        pass->depth_stencil_attachment = vara_allocate(sizeof(RenderPassAttachment));
        vara_copy_memory(
            pass->depth_stencil_attachment,
            config->depth_stencil_attachment,
            sizeof(RenderPassAttachment)
        );
    }

    pass->backend = backend;

    pass->packet_capacity = 256;
    pass->packets = vara_allocate(sizeof(RenderPacket) * pass->packet_capacity);
    pass->command_buffer = render_cmd_buffer_create();

    if (pass->backend->render_pass.create) {
        if (!pass->backend->render_pass.create(pass, config)) {
            ERROR("Failed to create render pass named('%s')", config->name);
            render_pass_destroy(pass);
            return NULL;
        }
    }

    return pass;
}

RenderPass* render_pass_create(Renderer* renderer, const RenderPassConfig* config) {
    return _render_pass_create(renderer->backend, config);
}

void render_pass_destroy(RenderPass* pass) {
    if (pass) {
        if (pass->backend->render_pass.destroy) {
            pass->backend->render_pass.destroy(pass);
        }
        render_cmd_buffer_destroy(pass->command_buffer);
        vara_free(pass->packets, sizeof(RenderPacket) * pass->packet_count);
        if (pass->color_attachments) {
            vara_free(
                pass->color_attachments, sizeof(RenderPassAttachment) * pass->color_attachment_count
            );
        }
        if (pass->depth_stencil_attachment) {
            vara_free(pass->depth_stencil_attachment, sizeof(RenderPassAttachment));
        }
        vara_free(pass, sizeof(RenderPass));
    }
}

void render_pass_begin(RenderPass* pass, RenderTarget* target) {
    if (!render_pass_validate_target(pass, target)) {
        return;
    }

    pass->packet_count = 0;
    render_cmd_buffer_reset(pass->command_buffer);
    render_cmd_begin_pass(pass->command_buffer, pass, target);
    render_cmd_set_viewport(pass->command_buffer, target->width, target->height);
    render_cmd_set_scissor(pass->command_buffer, target->width, target->height);
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
        void* resized_array = vara_allocate(sizeof(RenderPacket) * pass->packet_capacity);
        vara_copy_memory(resized_array, pass->packets, sizeof(RenderPacket) * pass->packet_count);
        vara_free(pass->packets, sizeof(RenderPacket) * pass->packet_count);
        pass->packets = resized_array;
    }
    pass->packets[pass->packet_count++] = *packet;
}
