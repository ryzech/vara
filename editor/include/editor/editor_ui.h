#pragma once

#include <vara/core/defines.h>

typedef struct Editor Editor;
typedef struct EditorTheme EditorTheme;

struct Panel;
struct PanelType;
struct Font;

struct EditorTheme {
    Vector4 panel_background_color;
    struct Font* default_font;
};

struct Editor {
    struct Panel* root;
    // Should each panel store this as a boolean?
    struct Panel* hovered_panel;
    struct Panel* hovered_split;
    EditorTheme theme;
    struct PanelType* registered_types;
    u32 type_count;
    u32 max_types;
};

b8 editor_ui_create(void);
void editor_ui_destroy(void);

void editor_ui_set_bounds(Vector2 min, Vector2 max);
void editor_ui_update(f32 delta);
void editor_ui_draw(void);

b8 editor_panel_register_type(struct PanelType type);
struct PanelType* editor_panel_get_type(const char* id);

struct Panel* editor_add_panel(struct PanelType* type);
struct Panel* editor_get_hovered_panel(void);
struct Panel* editor_get_hovered_split(void);
struct Panel* editor_get_root(void);