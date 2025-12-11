#pragma once

#include <flecs.h>
#include <vara/core/math/types.h>

typedef struct TransformComponent TransformComponent;
typedef struct SpriteComponent SpriteComponent;

struct Texture;

struct TransformComponent {
    Vector3 translation;
    Vector3 rotation;
    Vector3 scale;
};

struct SpriteComponent {
    struct Texture* texture;
    Vector4 color;
    i32 z_index;
};

// Declare components so the type is accessible.
// Different than registering.
ECS_COMPONENT_DECLARE(TransformComponent);
ECS_COMPONENT_DECLARE(SpriteComponent);