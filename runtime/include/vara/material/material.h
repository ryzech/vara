#pragma once

#include <vara/core/math/types.h>

typedef struct Material Material;

struct Shader;
struct Texture;

struct Material {
    struct Shader* shader;
    Matrix4 model;
    Matrix4 view_projection;
    struct Texture* textures[16];
    u32 texture_count;
};