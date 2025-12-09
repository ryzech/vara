#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

#include "vara/scene/node.h"

typedef struct UINode UINode;
typedef struct TextNode TextNode;

struct Font;

// This likely shouldn't have a color, and shouldn't really be rendered, but for testing
// I will keep it this way.
// TODO: make children positions relative to their parent
// i.e. a parent with position 100, 100, it's child will be at 0, 0 relative to the parent
// but 100, 100 globally
struct UINode {
    Node base;
    Transform2D local_transform;
    Vector4 color;
};

struct TextNode {
    UINode base;
    char* text;
    struct Font* font;
};

UINode* ui_node_create(Rect rect, Vector4 color, const char* name);
TextNode* text_node_create(
    Vector2 position, const char* text, struct Font* font, Vector4 color, const char* name
);