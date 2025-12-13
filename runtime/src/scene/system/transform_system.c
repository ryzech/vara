#include <vara/core/defines.h>
#include <vara/core/math/math.h>

#include "vara/scene/component.h"
#include "vara/scene/node.h"
#include "vara/scene/system/transform_system.h"

static void update_transform_hierarchy(Node node, const Matrix4* parent) {
    const TransformComponent* transform_component = node_get_component(node, TransformComponent);
    // TODO: Handle rotation.
    const Matrix4 local_matrix = mat4_mul(
        mat4_translation(transform_component->translation), mat4_scale(transform_component->scale)
    );

    Matrix4 world_matrix;
    if (parent != NULL) {
        world_matrix = mat4_mul(*parent, local_matrix);
    } else {
        world_matrix = local_matrix;
    }

    const WorldTransformComponent world_transform = {
        .matrix = world_matrix,
    };
    node_set_component(node, WorldTransformComponent, &world_transform);

    ecs_iter_t it = ecs_children(node.world, node.entity);
    while (ecs_children_next(&it)) {
        for (i32 i = 0; i < it.count; i++) {
            const Node child = node_of(it.world, it.entities[i]);
            update_transform_hierarchy(child, &world_matrix);
        }
    }
}

void TransformSystem(ecs_iter_t* it) {
    for (i32 i = 0; i < it->count; i++) {
        const Node node = node_of(it->world, it->entities[i]);
        update_transform_hierarchy(node, NULL);
    }
}
