#pragma once

#include <vara/core/defines.h>

typedef struct Node Node;

struct Node {
    Node* parent;
    Node** children;
    u32 child_count;
    u32 child_capacity;

    void (*update)(Node* self, f32 delta);
    void (*draw)(Node* self);

    char* name;
};

Node* node_create(size_t size, const char* name);
void node_destroy(Node* node);

void node_add_child(Node* parent, Node* child);
void node_remove_child(Node* parent, Node* child);