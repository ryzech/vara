#include "vara/ui/ui.h"
#include "vara/renderer2d/renderer2d.h"

static void ui_node_draw(Node* self) {
    const UINode* node = (UINode*)self;
    renderer2d_draw_rect(node->rect, node->color, 0);
}

UINode* ui_node_create(Rect rect, Vector4 color, const char* name) {
    UINode* node = (UINode*)node_create(sizeof(UINode), name);
    node->base.draw = ui_node_draw;
    node->rect = rect;
    node->color = color;
    return node;
}
