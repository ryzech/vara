#pragma once

typedef struct Material Material;

struct Shader;

struct Material {
    struct Shader* shader;
};