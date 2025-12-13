#include <vara/core/logger.h>

#include "vara/scene/component.h"
#include "vara/scene/node.h"
#include "vara/scene/scene.h"

Node node_create(Scene scene, const char* name) {
    const ecs_entity_t entity_id = ecs_new(scene.world);
    TRACE("Created entity %u ('%s')", entity_id, name ? name : "Unnamed");
    if (name) {
        ecs_set_name(scene.world, entity_id, name);
    }

    // Have a wrapper function for adding/editing components.
    const TransformComponent default_transform = {
        .scale = {1.0f, 1.0f, 1.0f},
    };
    ecs_set_id(
        scene.world,
        entity_id,
        ecs_id(TransformComponent),
        sizeof(TransformComponent),
        &default_transform
    );

    return (Node){
        .entity = entity_id,
        .world = scene.world,
    };
}

Node node_of(ecs_world_t* world, ecs_entity_t entity_id) {
    return (Node){
        .entity = entity_id,
        .world = world,
    };
}

void node_destroy(Node node) {
    if (!node_exists(node)) {
        return;
    }

    ecs_delete(node.world, node.entity);
}

b8 node_exists(Node node) {
    return ecs_is_valid(node.world, node.entity) && ecs_is_alive(node.world, node.entity);
}

void node_add_child(Node parent, Node child) {
    if (!node_exists(parent) || !node_exists(child)) {
        return;
    }

    ecs_add_pair(child.world, child.entity, EcsChildOf, parent.entity);
}

void node_remove_child(Node parent, Node child) {
    if (!node_exists(parent) || !node_exists(child)) {
        return;
    }

    ecs_remove_pair(child.world, child.entity, EcsChildOf, parent.entity);
}

b8 node_has_parent(Node node) {
    if (!node_exists(node)) {
        return false;
    }

    return ecs_has_pair(node.world, node.entity, EcsChildOf, EcsWildcard);
}

const char* node_get_name(Node node) {
    if (node_exists(node)) {
        return ecs_get_name(node.world, node.entity);
    }

    return "Unnamed";
}
