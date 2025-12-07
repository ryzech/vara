#include <vara/core/util/string.h>

#include "vara/renderer/font.h"
#include "vara/renderer2d/renderer2d.h"
#include "vara/ui/ui.h"

static void ui_node_draw(Node* self) {
    const UINode* node = (UINode*)self;
    renderer2d_draw_rect(node->rect, node->color, 0);
}

static void text_node_draw(Node* self) {
    const TextNode* node = (TextNode*)self;
    renderer2d_draw_text(node->base.rect.position, node->text, node->font, node->base.color, 0);
}

UINode* ui_node_create(Rect rect, Vector4 color, const char* name) {
    UINode* node = (UINode*)node_create(sizeof(UINode), name);
    node->base.draw = ui_node_draw;
    node->rect = rect;
    node->color = color;
    return node;
}

TextNode* text_node_create(
    Rect rect, const char* text, struct Font* font, Vector2 scale, Vector4 color, const char* name
) {
    TextNode* node = (TextNode*)node_create(sizeof(TextNode), name);
    node->base.base.draw = text_node_draw;
    node->base.rect = rect;
    node->base.scale = scale;
    node->base.color = color;
    node->text = string_duplicate(text);
    node->font = font;
    return node;
}
