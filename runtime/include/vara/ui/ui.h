#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

#include "vara/scene/node.h"

typedef struct UINode UINode;

typedef struct TextNode TextNode;

struct Font;

// This likely shouldn't have a color, and shouldn't really be rendered, but for testing
// I will keep it this way.
// TODO: transform type
struct UINode {
    Node base;
    Rect rect;
    Vector2 scale;
    Vector4 color;
};

struct TextNode {
    UINode base;
    char* text;
    struct Font* font;
};

UINode* ui_node_create(Rect rect, Vector4 color, const char* name);
TextNode* text_node_create(
    Rect rect, const char* text, struct Font* font, Vector2 scale, Vector4 color, const char* name
);