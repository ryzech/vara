#include <vara/core/platform/platform.h>

#include "editor/editor_panel.h"
#include "editor/editor_ui.h"

Panel* panel_create(PanelType* type) {
    Panel* panel = platform_allocate(sizeof(Panel));
    platform_zero_memory(panel, sizeof(Panel));

    panel->split = false;
    panel->type = type;

    if (type->data_size > 0) {
        panel->data = platform_allocate(type->data_size);
        platform_zero_memory(panel->data, type->data_size);
    }

    if (type->init) {
        if (!type->init(panel)) {
            if (panel->data) {
                platform_free(panel->data);
            }
            platform_free(panel);
            return NULL;
        }
    }

    return panel;
}

void panel_destroy(Panel* panel) {
    if (!panel) {
        return;
    }

    if (panel->split) {
        panel_destroy(panel->children[0]);
        panel_destroy(panel->children[1]);
    } else {
        if (panel->type->destroy) {
            panel->type->destroy(panel);
        }
        if (panel->data) {
            platform_free(panel->data);
        }
    }

    platform_free(panel);
}

Panel* panel_add(Panel* target, PanelType* type, b8 vertical) {
    if (!target) {
        return NULL;
    }

    Panel* new_panel = panel_create(type);
    if (!new_panel) {
        return NULL;
    }

    if (target->split) {
        panel_destroy(new_panel);
        return NULL;
    }

    Panel* moved_panel = panel_create(target->type);
    if (!moved_panel) {
        panel_destroy(new_panel);
        return NULL;
    }

    moved_panel->data = target->data;
    moved_panel->focused = target->focused;
    moved_panel->bounds = target->bounds;

    target->data = NULL;
    target->type = editor_panel_get_type("SplitPanel");
    target->split = true;
    target->split_vertical = vertical;
    target->split_ratio = 0.5f;

    target->children[0] = moved_panel;
    target->children[1] = new_panel;
    moved_panel->parent = target;
    new_panel->parent = target;

    panel_calculate(target);
    return new_panel;
}

void panel_remove(Panel* panel) {
    if (!panel || !panel->parent) {
        return;
    }

    Panel* parent = panel->parent;
    Panel* sibling;
    if (parent->children[0] == panel) {
        sibling = parent->children[1];
    } else {
        sibling = parent->children[0];
    }

    Panel* grandparent = parent->parent;
    sibling->parent = grandparent;

    if (grandparent) {
        if (grandparent->children[0] == parent) {
            grandparent->children[0] = sibling;
        } else {
            grandparent->children[1] = sibling;
        }
    }

    platform_free(parent);
}

void panel_calculate(Panel* panel) {
    if (!panel || !panel->split) {
        return;
    }

    Panel* child_a = panel->children[0];
    Panel* child_b = panel->children[1];

    if (!child_a && !child_b) {
        return;
    }

    if (!child_a || !child_b) {
        Panel* child = child_a ? child_a : child_b;
        child->bounds = panel->bounds;
        if (child->split) {
            panel_calculate(child);
        }
        return;
    }

    PanelBounds child_a_bounds = panel->bounds;
    PanelBounds child_b_bounds = panel->bounds;

    if (panel->split_vertical) {
        const f32 height = panel->bounds.max.y - panel->bounds.min.y;
        const f32 split_height = panel->bounds.min.y + height * panel->split_ratio;
        child_a_bounds.max.y = split_height;
        child_b_bounds.min.y = split_height;
    } else {
        const f32 width = panel->bounds.max.x - panel->bounds.min.x;
        const f32 split_width = panel->bounds.min.x + width * panel->split_ratio;
        child_a_bounds.max.x = split_width;
        child_b_bounds.min.x = split_width;
    }

    child_a->bounds = child_a_bounds;
    child_b->bounds = child_b_bounds;

    if (child_a->split) {
        panel_calculate(child_a);
    }

    if (child_b->split) {
        panel_calculate(child_b);
    }
}
