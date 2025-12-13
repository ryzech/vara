#include <vara/core/defines.h>
#include <vara/core/math/math.h>

#include "vara/scene/component.h"
#include "vara/scene/node.h"
#include "vara/scene/system/transform_system.h"

static Matrix4 calculate_local_matrix(const TransformComponent* component) {
    const Matrix4 translation = mat4_translation(component->translation);
    const Matrix4 rotation = mat4_rotate(component->rotation);
    const Matrix4 scale = mat4_scale(component->scale);

    const Matrix4 translation_rotation = mat4_mul(translation, rotation);
    const Matrix4 final = mat4_mul(translation_rotation, scale);

    return final;
}

static void update_transform_hierarchy(Node node, const Matrix4* parent) {
    const TransformComponent* transform_component = node_get_component(node, TransformComponent);
    const Matrix4 local_matrix = calculate_local_matrix(transform_component);

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
