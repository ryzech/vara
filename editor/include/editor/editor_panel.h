#pragma once

#include <vara/core/math/types.h>

typedef struct Panel Panel;
typedef struct PanelBounds PanelBounds;
typedef struct PanelType PanelType;
typedef enum PanelNodeType PanelNodeType;
typedef enum PanelSplitDirection PanelSplitDirection;

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

enum PanelNodeType {
    NODE_LEAF,
    NODE_SPLIT
};

enum PanelSplitDirection {
    SPLIT_HORIZONTAL,
    SPLIT_VERTICAL
};

struct Panel {
    PanelNodeType node_type;
    PanelType* type;
    void* data;
    PanelBounds bounds;

    Panel* parent;
    Panel* children[2];
    PanelSplitDirection direction;
    f32 split_ratio;
};

Panel* panel_create(PanelType* type);
void panel_destroy(Panel* panel);

Panel* panel_add(Panel* target, PanelType* type, PanelSplitDirection direction);
void panel_remove(Panel* panel);
void panel_calculate(Panel* panel, PanelBounds bounds);