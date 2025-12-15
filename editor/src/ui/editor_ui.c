#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

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

    editor->max_panels = 16;
    editor->panels = platform_allocate(sizeof(Panel) * editor->max_panels);

    return true;
}

void editor_ui_destroy(void) {
    if (!editor) {
        return;
    }

    platform_free(editor->panels);
    platform_free(editor);
}

void editor_ui_update(f32 delta) {
    if (!editor) {
        return;
    }

    for (u32 i = 0; i < editor->panel_count; i++) {
        Panel* panel = &editor->panels[i];
        if (!panel->visible) {
            continue;
        }

        if (panel->update) {
            panel->update(panel, delta);
        }
    }
}

void editor_ui_draw(void) {
    if (!editor) {
        return;
    }

    for (u32 i = 0; i < editor->panel_count; i++) {
        Panel* panel = &editor->panels[i];
        if (!panel->visible) {
            continue;
        }

        if (panel->draw) {
            panel->draw(panel);
        }
    }
}

Panel* editor_panel_register(PanelConfig config) {
    if (!editor) {
        return NULL;
    }

    if (editor->panel_count >= editor->max_panels) {
        editor->max_panels *= 2;
        void* resized_capacity = platform_allocate(sizeof(Panel) * editor->max_panels);
        platform_copy_memory(resized_capacity, editor->panels, sizeof(Panel) * editor->max_panels);
        platform_free(editor->panels);
        editor->panels = resized_capacity;
    }

    Panel* panel = &editor->panels[editor->panel_count++];
    panel->id = config.id;
    panel->title = config.title;
    panel->visible = true;
    panel->focused = false;
    panel->init = config.init;
    panel->update = config.update;
    panel->draw = config.draw;
    panel->destroy = config.destroy;

    TRACE("Registering panel id('%s')", panel->id);

    if (config.data_size > 0) {
        panel->data = platform_allocate(config.data_size);
        platform_zero_memory(panel->data, config.data_size);
        panel->data_size = config.data_size;
    }

    if (panel->init) {
        if (!panel->init(panel)) {
            ERROR("Failed to initialize panel id('%s')", panel->id);
            // Need to free memory and resources here.
            return NULL;
        }
    }

    return panel;
}

void editor_panel_remove(const char* id) {
    // TODO: implement unregistering the panel/freeing memory.
}
