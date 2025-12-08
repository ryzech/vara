#include <vara/core/util/string.h>

#include "vara/renderer/font.h"
#include "vara/renderer2d/renderer2d.h"
#include "vara/ui/ui.h"

static void ui_node_draw(Node* self) {
    const UINode* node = (UINode*)self;
    const Rect rect = {node->transform.position, node->transform.size};
    renderer2d_draw_rect(rect, node->color, 0);
}

static void text_node_draw(Node* self) {
    const TextNode* node = (TextNode*)self;
    renderer2d_draw_text(node->base.transform.position, node->text, node->font, node->base.color, 0);
}

UINode* ui_node_create(Transform2D transform, Vector4 color, const char* name) {
    UINode* node = (UINode*)node_create(sizeof(UINode), name);
    node->base.draw = ui_node_draw;
    node->transform = transform;
    node->color = color;
    return node;
}

TextNode* text_node_create(
    Transform2D transform, const char* text, struct Font* font, Vector4 color, const char* name
) {
    TextNode* node = (TextNode*)node_create(sizeof(TextNode), name);
    node->base.base.draw = text_node_draw;
    node->base.transform = transform;
    node->base.color = color;
    node->text = string_duplicate(text);
    node->font = font;
    return node;
}
