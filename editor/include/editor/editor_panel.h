#pragma once

#include <vara/core/math/types.h>

typedef struct Panel Panel;
typedef struct PanelBounds PanelBounds;
typedef struct PanelType PanelType;

struct PanelBounds {
    Vector2 min, max;
};

struct PanelType {
    const char* id;
    const char* title;
    size_t data_size;

    b8 (*init)(Panel* self);
    void (*update)(Panel* self, f32 delta);
    void (*draw)(Panel* self);
    void (*destroy)(Panel* self);
};

struct Panel {
    PanelType* type;
    void* data;
    b8 focused;
    PanelBounds bounds;

    Panel* parent;
    Panel* children[2];
    b8 split;
    b8 split_vertical;
    f32 split_ratio;
};

Panel* panel_create(PanelType* type);
void panel_destroy(Panel* panel);

Panel* panel_add(Panel* target, PanelType* type, b8 vertical);
void panel_remove(Panel* panel);
void panel_calculate(Panel* panel);