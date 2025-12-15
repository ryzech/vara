#pragma once

#include <vara/core/defines.h>

typedef struct Editor Editor;

struct Panel;
struct PanelConfig;

struct Editor {
    struct Panel* panels;
    u32 panel_count;
    u32 max_panels;

    struct Panel* focused_panel;
    struct Panel* hovered_panel;
};

b8 editor_ui_create(void);
void editor_ui_destroy(void);

void editor_ui_update(f32 delta);
void editor_ui_draw(void);

struct Panel* editor_panel_register(struct PanelConfig config);
void editor_panel_remove(const char* id);