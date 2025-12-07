#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

#include "vara/scene/node.h"

typedef struct UINode UINode;

typedef struct TextNode TextNode;

struct UINode {
    Node base;
    Rect rect;
    Vector4 color;
};

struct TextNode {
    UINode base;
    char* text;
};

UINode* ui_node_create(Rect rect, Vector4 color, const char* name);