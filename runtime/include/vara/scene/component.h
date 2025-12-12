#pragma once

#include <flecs.h>
#include <vara/core/math/types.h>

typedef struct TransformComponent TransformComponent;
typedef struct WorldTransformComponent WorldTransformComponent;
typedef struct SpriteComponent SpriteComponent;

struct Texture;

struct TransformComponent {
    Vector3 translation;
    Vector3 rotation;
    Vector3 scale;
};

// Need a system to calculate this from the parent transform (or just be equal to the transform if
// no parent)
struct WorldTransformComponent {
    Matrix4 matrix;
};

struct SpriteComponent {
    struct Texture* texture;
    Vector4 color;
    i32 z_index;
};

// Declare components so the type is accessible.
// Different than registering.
ECS_COMPONENT_DECLARE(TransformComponent);
ECS_COMPONENT_DECLARE(WorldTransformComponent);
ECS_COMPONENT_DECLARE(SpriteComponent);