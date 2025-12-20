#pragma once

#include <vara/core/math/types.h>

typedef struct Panel Panel;
typedef struct PanelBounds PanelBounds;
typedef struct PanelType PanelType;
typedef enum PanelNodeType PanelNodeType;

typedef struct SplitInfo SplitInfo;
typedef enum SplitDirection SplitDirection;

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

enum SplitDirection {
    SPLIT_HORIZONTAL,
    SPLIT_VERTICAL
};

struct SplitInfo {
    PanelBounds bounds;
    SplitDirection direction;
};

struct Panel {
    PanelNodeType node_type;
    PanelType* type;
    void* data;
    PanelBounds bounds;

    Panel* parent;
    Panel* children[2];
    SplitDirection direction;
    f32 split_ratio;
};

Panel* panel_create(PanelType* type);
void panel_destroy(Panel* panel);

Panel* panel_add(Panel* target, PanelType* type, SplitDirection direction);
SplitInfo panel_get_split(Panel* panel);
void panel_set_split_ratio(Panel* panel, f32 split_ratio);
void panel_remove(Panel* panel);
void panel_calculate(Panel* panel, PanelBounds bounds);