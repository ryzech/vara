#pragma once

typedef struct Material Material;

struct Texture;

struct Material {
    struct Texture* textures[16];
    u32 texture_count;
};