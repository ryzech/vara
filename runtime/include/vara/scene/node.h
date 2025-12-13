#pragma once

#include <flecs.h>
#include <vara/core/defines.h>

typedef struct Node Node;

struct Scene;

struct Node {
    ecs_entity_t entity;
    ecs_world_t* world;
};

Node node_create(struct Scene scene, const char* name);
Node node_of(ecs_world_t* world, ecs_entity_t entity_id);
void node_destroy(Node node);

b8 node_exists(Node node);
void node_add_child(Node parent, Node child);
void node_remove_child(Node parent, Node child);

b8 node_has_parent(Node node);

const char* node_get_name(Node node);

// For now, we're using macros to add/modify components.
#define node_add_component(node, T)    ecs_add_id(node.world, node.entity, ecs_id(T))
#define node_remove_component(node, T) ecs_remove_id(node.world, node.entity, ecs_id(T))
#define node_set_component(node, T, value)                                                         \
    ecs_set_id(node.world, node.entity, ecs_id(T), sizeof(T), value)
#define node_get_component(node, T) ecs_get_mut(node.world, node.entity, T)
