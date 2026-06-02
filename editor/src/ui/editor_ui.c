#include <vara/application/application.h>
#include <vara/core/input/input.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>
#include <vara/core/util/string.h>
#include <vara/renderer/buffer.h>
#include <vara/renderer/render_command.h>
#include <vara/renderer/render_packet.h>
#include <vara/renderer/render_pass.h>
#include <vara/renderer/render_pipeline.h>

#include "editor/editor_panel.h"
#include "editor/editor_ui.h"

static Editor* editor;

extern RenderPass* ui_pass;
extern RenderPipeline* ui_pipeline;
extern Buffer* ui_vertex_buffer;
extern Buffer* ui_index_buffer;

typedef struct UIPushConstants {
    Matrix4 projection;
    Matrix4 model;
} UIPushConstants;

b8 editor_ui_create(Renderer* renderer) {
    editor = platform_allocate(sizeof(Editor));
    platform_zero_memory(editor, sizeof(Editor));

    if (!editor) {
        FATAL("Failed to create Editor Context!");
        return false;
    }

    // Start with 64 type capacity.
    editor->max_types = 64;
    editor->registered_types = platform_allocate(sizeof(PanelType) * editor->max_types);
    platform_zero_memory(editor->registered_types, sizeof(PanelType) * editor->max_types);

    const PanelType test_type = {
        .id = "TestPanel",
    };
    editor_panel_register_type(test_type);

    editor->root = platform_allocate(sizeof(Panel));
    platform_zero_memory(editor->root, sizeof(Panel));
    editor->root->node_type = NODE_SPLIT;
    editor->root->direction = SPLIT_HORIZONTAL;
    editor->root->split_ratio = 0.5f;

    PanelType* test_panel = editor_panel_get_type("TestPanel");
    editor->root->children[0] = panel_create(test_panel);
    editor->root->children[0]->parent = editor->root;
    editor->root->children[1] = panel_create(test_panel);
    editor->root->children[1]->parent = editor->root;

    const Vector2i window_size = platform_window_get_size(application_get_window());
    editor_ui_set_bounds(vec2_zero(), (Vector2){window_size.x, window_size.y});

    panel_add(editor->root->children[1], test_panel, SPLIT_VERTICAL);

    return true;
}

void editor_ui_destroy(void) {
    if (!editor) {
        return;
    }

    panel_destroy(editor->root);
    platform_free(editor->registered_types);
    platform_free(editor);
}

void editor_ui_set_bounds(Vector2 min, Vector2 max) {
    if (!editor || !editor->root) {
        return;
    }

    const PanelBounds bounds = {
        .min = min,
        .max = max,
    };
    panel_calculate(editor->root, bounds);
}

void editor_ui_update(f32 delta) {
    if (!editor) {
        return;
    }

    Panel* hovered_split = editor_get_hovered_split();
    editor->hovered_split = hovered_split;
    if (hovered_split && !editor->resizing_split) {
        if (hovered_split->direction == SPLIT_VERTICAL) {
            platform_window_set_cursor(application_get_window(), CURSOR_VERTICAL_RESIZE);
        } else {
            platform_window_set_cursor(application_get_window(), CURSOR_HORIZONTAL_RESIZE);
        }

        if (input_is_mouse_down(MOUSE_BUTTON_LEFT)) {
            editor->resizing_split = editor->hovered_split;
        }
    }

    if (input_is_mouse_down(MOUSE_BUTTON_LEFT) && editor->resizing_split) {
        const Vector2 mouse = input_get_mouse_position();
        f32 resize_ratio;
        if (editor->resizing_split->direction == SPLIT_VERTICAL) {
            const f32 height =
                editor->resizing_split->bounds.max.y - editor->resizing_split->bounds.min.y;
            const f32 split_height = mouse.y - editor->resizing_split->bounds.min.y;
            resize_ratio = split_height / height;
        } else {
            const f32 width =
                editor->resizing_split->bounds.max.x - editor->resizing_split->bounds.min.x;
            const f32 split_width = mouse.x - editor->resizing_split->bounds.min.x;
            resize_ratio = split_width / width;
        }
        panel_set_split_ratio(editor->resizing_split, resize_ratio);
        panel_calculate(editor->resizing_split, editor->resizing_split->bounds);
    } else {
        editor->resizing_split = NULL;
    }

    if (!hovered_split && !editor->resizing_split) {
        platform_window_set_cursor(application_get_window(), CURSOR_NORMAL);
        editor->hovered_panel = editor_get_hovered_panel();
    } else {
        editor->hovered_panel = NULL;
    }
}

static void editor_panel_draw(Panel* panel) {
    if (!panel) {
        return;
    }

    if (panel->node_type == NODE_SPLIT) {
        editor_panel_draw(panel->children[0]);
        editor_panel_draw(panel->children[1]);
    } else {
        Vector2 size = {
            panel->bounds.max.x - panel->bounds.min.x,
            panel->bounds.max.y - panel->bounds.min.y,
        };

        Vector4 background;
        if (panel == editor->hovered_panel) {
            background = (Vector4){0.30f, 0.30f, 0.30f, 1.0f};
        } else {
            background = (Vector4){0.25f, 0.25f, 0.25f, 1.0f};
        }

        Geometry2D quad = geometry_generate_quad(size, vec2_zero(), vec2_one(), background);
        buffer_set_data(ui_vertex_buffer, quad.vertices, sizeof(Vertex) * quad.vertex_count, 0);
        buffer_set_data(ui_index_buffer, quad.indices, sizeof(u32) * quad.index_count, 0);

        const Vector2i window_size = platform_window_get_size(application_get_window());
        Matrix4 projection =
            mat4_ortho(0.0f, (f32)window_size.x, (f32)window_size.y, 0.0f, -1.0f, 1.0f);
        Matrix4 model = mat4_translation(vec3(panel->bounds.min.x, panel->bounds.min.y, 0.0f));

        UIPushConstants push = {
            .projection = projection,
            .model = model,
        };

        render_cmd_push_constants(
            ui_pass->command_buffer, ui_pipeline, 0, sizeof(UIPushConstants), &push
        );
        RenderPacket packet = {
            .pipeline = ui_pipeline,
            .vertex_buffer = ui_vertex_buffer,
            .index_buffer = ui_index_buffer,
            .index_count = quad.index_count,
            .vertex_count = quad.vertex_count,
        };
        render_pass_submit(ui_pass, &packet);
        geometry_destroy(&quad);

        if (panel->type) {
            if (panel->type->draw) {
                panel->type->draw(panel);
            }
        }
    }
}

void editor_ui_draw(void) {
    if (!editor) {
        return;
    }

    editor_panel_draw(editor->root);
}

b8 editor_panel_register_type(PanelType type) {
    if (!editor) {
        return false;
    }

    if (editor->type_count >= editor->max_types) {
        editor->max_types *= 2;
        void* resized_capacity = platform_allocate(sizeof(PanelType) * editor->max_types);
        platform_copy_memory(
            resized_capacity, editor->registered_types, sizeof(PanelType) * editor->type_count
        );
        platform_free(editor->registered_types);
        editor->registered_types = resized_capacity;
    }

    editor->registered_types[editor->type_count++] = type;
    return true;
}

PanelType* editor_panel_get_type(const char* id) {
    for (u32 i = 0; i < editor->type_count; i++) {
        if (strings_equal(editor->registered_types[i].id, id)) {
            return &editor->registered_types[i];
        }
    }
    return NULL;
}

Panel* editor_add_panel(PanelType* type) {
    return NULL;
}

static Panel* find_hovered_panel(Panel* panel) {
    if (!panel) {
        return NULL;
    }

    const Vector2 mouse = input_get_mouse_position();
    if (mouse.x < panel->bounds.min.x || mouse.x > panel->bounds.max.x
        || mouse.y < panel->bounds.min.y || mouse.y > panel->bounds.max.y) {
        return NULL;
    }

    if (panel->node_type == NODE_LEAF) {
        return panel;
    }

    Panel* first_child = find_hovered_panel(panel->children[0]);
    if (first_child) {
        return first_child;
    }
    return find_hovered_panel(panel->children[1]);
}

Panel* editor_get_hovered_panel(void) {
    if (!editor || !editor->root) {
        return NULL;
    }

    return find_hovered_panel(editor->root);
}

static Panel* find_hovered_split(Panel* panel) {
    if (!panel || panel->node_type != NODE_SPLIT) {
        return NULL;
    }

    const Vector2 mouse = input_get_mouse_position();
    if (mouse.x < panel->bounds.min.x || mouse.x > panel->bounds.max.x
        || mouse.y < panel->bounds.min.y || mouse.y > panel->bounds.max.y) {
        return NULL;
    }

    const SplitInfo info = panel_get_split(panel);
    if (mouse.x >= info.bounds.min.x && mouse.x <= info.bounds.max.x && mouse.y >= info.bounds.min.y
        && mouse.y <= info.bounds.max.y) {
        return panel;
    }

    Panel* first_child = find_hovered_split(panel->children[0]);
    if (first_child) {
        return first_child;
    }
    return find_hovered_split(panel->children[1]);
}

Panel* editor_get_hovered_split(void) {
    if (!editor || !editor->root) {
        return NULL;
    }

    return find_hovered_split(editor->root);
}

Panel* editor_get_root(void) {
    if (!editor || !editor->root) {
        return NULL;
    }

    return editor->root;
}
