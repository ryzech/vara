#include <vara/application/application.h>
#include <vara/core/defines.h>
#include <vara/core/math/math.h>
#include <vara/core/platform/platform_window.h>

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

static Matrix4 calculate_rect_transform_matrix(
    const RectTransformComponent* rect_transform, const Rect* parent_rect
) {
    const Vector2 anchor_min = rect_transform->anchor_min;
    const Vector2 anchor_max = rect_transform->anchor_max;
    const Vector2 offset_min = rect_transform->offset_min;
    const Vector2 offset_max = rect_transform->offset_max;

    const Vector2 anchored_min = {
        parent_rect->position.x + parent_rect->size.x * anchor_min.x + offset_min.x,
        parent_rect->position.y + parent_rect->size.y * anchor_min.y + offset_min.y
    };
    const Vector2 anchored_max = {
        parent_rect->position.x + parent_rect->size.x * anchor_max.x + offset_max.x,
        parent_rect->position.y + parent_rect->size.y * anchor_max.y + offset_max.y
    };

    const Rect rect = {
        .position = anchored_min,
        .size = {anchored_max.x - anchored_min.x, anchored_max.y - anchored_min.y}
    };

    const Vector3 translation = {
        rect.position.x,
        rect.position.y,
        0.0f,
    };
    const Vector3 scale = {
        rect.size.x,
        rect.size.y,
        1.0f,
    };

    const Matrix4 translation_matrix = mat4_translation(translation);
    const Matrix4 scale_matrix = mat4_scale(scale);

    return mat4_mul(translation_matrix, scale_matrix);
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

static void update_rect_transform_hierarchy(Node node, const Rect* parent) {
    RectTransformComponent* rect_transform = node_get_component(node, RectTransformComponent);

    Rect parent_rect;
    if (parent) {
        parent_rect = *parent;
    } else {
        // TODO: handle with a Canvas component or similar, take a reference resolution and scale
        VaraWindow* window = application_get_window();
        const Vector2i size = platform_window_get_size(window);
        parent_rect = (Rect){
            vec2_zero(),
            {(f32)size.x, (f32)size.y},
        };
    }

    const Matrix4 world_matrix = calculate_rect_transform_matrix(rect_transform, &parent_rect);
    const Vector2 anchor_min = rect_transform->anchor_min;
    const Vector2 anchor_max = rect_transform->anchor_max;
    const Vector2 offset_min = rect_transform->offset_min;
    const Vector2 offset_max = rect_transform->offset_max;

    rect_transform->rect = (Rect){
        .position =
            {parent_rect.position.x + parent_rect.size.x * anchor_min.x + offset_min.x,
             parent_rect.position.y + parent_rect.size.y * anchor_min.y + offset_min.y},
        .size =
            {parent_rect.size.x * (anchor_max.x - anchor_min.x) + (offset_max.x - offset_min.x),
             parent_rect.size.y * (anchor_max.y - anchor_min.y) + (offset_max.y - offset_min.y)},
    };

    const WorldTransformComponent world_transform = {
        .matrix = world_matrix,
    };
    node_set_component(node, WorldTransformComponent, &world_transform);

    ecs_iter_t it = ecs_children(node.world, node.entity);
    while (ecs_children_next(&it)) {
        for (i32 i = 0; i < it.count; i++) {
            const Node child = node_of(it.world, it.entities[i]);
            // TODO: Check if child has component
            update_rect_transform_hierarchy(child, &parent_rect);
        }
    }
}

void TransformSystem(ecs_iter_t* it) {
    for (i32 i = 0; i < it->count; i++) {
        const Node node = node_of(it->world, it->entities[i]);
        update_transform_hierarchy(node, NULL);
    }
}

void RectTransformSystem(ecs_iter_t* it) {
    for (i32 i = 0; i < it->count; i++) {
        const Node node = node_of(it->world, it->entities[i]);
        update_rect_transform_hierarchy(node, NULL);
    }
}
