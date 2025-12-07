#include <vara/core/platform/platform.h>
#include <vara/core/util/string.h>

#include "vara/scene/node.h"

#include "vara/core/logger.h"

Node* node_create(const size_t size, const char* name) {
    TRACE("Creating node('%s')", name);
    Node* node = platform_allocate(size);
    node->name = string_duplicate(name);
    return node;
}

void node_destroy(Node* node) {
    if (!node) {
        return;
    }

    if (node->child_count > 0) {
        for (u32 i = 0; i < node->child_count; i++) {
            node_destroy(node->children[i]);
        }
        platform_free(node->children);
    }

    if (node->name) {
        string_free(node->name);
    }

    platform_free(node);
}

void node_add_child(Node* parent, Node* child) {
    if (!parent || !child) {
        return;
    }

    // Mostly copied the logic from render_command buffers.
    // Could likely use some optimization later.
    if (parent->child_count >= parent->child_capacity) {
        const u32 old_capacity = parent->child_capacity;
        const u32 new_capacity = old_capacity == 0 ? 4 : old_capacity * 2;
        Node** resized_children = platform_allocate(new_capacity * sizeof(Node*));

        if (old_capacity > 0) {
            platform_copy_memory(resized_children, parent->children, old_capacity * sizeof(Node*));
            platform_free(parent->children);
        }

        parent->children = resized_children;
        parent->child_capacity = new_capacity;
    }

    TRACE("Adding child node('%s') to parent('%s')", child->name, parent->name);
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void node_remove_child(Node* parent, Node* child) {
    WARN("Not yet implemented.")
}

void node_update_tree(Node* root, f32 delta) {
    WARN("Not yet implemented.")
}

void node_draw_tree(Node* root) {
    WARN("Not yet implemented.")
}
