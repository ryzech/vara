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
Node node_of(struct Scene Scene, ecs_entity_t entity_id);
void node_destroy(Node node);

b8 node_exists(Node node);
void node_add_child(Node parent, Node child);
void node_remove_child(Node parent, Node child);

const char* node_get_name(Node node);