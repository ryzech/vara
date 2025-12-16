#include <vara/application/application.h>
#include <vara/core/logger.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>
#include <vara/core/util/string.h>
#include <vara/renderer2d/renderer2d.h>

#include "editor/editor_panel.h"
#include "editor/editor_ui.h"

static Editor* editor;

b8 editor_ui_create(void) {
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

    const PanelType split_type = {
        .id = "SplitPanel",
    };
    editor_panel_register_type(split_type);

    PanelType* split_panel = editor_panel_get_type("SplitPanel");
    editor->root = panel_create(split_panel);
    editor->root->split = true;
    editor->root->split_ratio = 0.5f;

    const Vector2i window_size = platform_window_get_size(application_get_window());
    editor_ui_set_bounds(vec2_zero(), (Vector2){window_size.x, window_size.y});

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

    editor->root->bounds.min = min;
    editor->root->bounds.max = max;
    panel_calculate(editor->root);
}

void editor_ui_update(f32 delta) {
    if (!editor) {
        return;
    }
}

static void editor_panel_draw(Panel* panel) {
    if (!panel) {
        return;
    }

    if (panel->split) {
        editor_panel_draw(panel->children[0]);
        editor_panel_draw(panel->children[1]);
    } else {
        const Vector2 size = {
            panel->bounds.max.x - panel->bounds.min.x,
            panel->bounds.max.y - panel->bounds.min.y,
        };
        const Vector4 background = {0.25f, 0.25f, 0.25f, 1.0f};
        renderer2d_draw_rect(panel->bounds.min, size, background, 0);
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
            resized_capacity, editor->registered_types, sizeof(Panel) * editor->max_types
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

Panel* editor_get_root(void) {
    if (!editor || !editor->root) {
        return NULL;
    }

    return editor->root;
}
