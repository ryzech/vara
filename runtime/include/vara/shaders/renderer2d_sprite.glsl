#[vertex]

#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec4 vColor;
layout (location = 1) out vec2 vTexCoord;

layout (push_constant) uniform UIPushConstants {
    mat4 uProjection;
    mat4 uModel;
} push;

void main() {
    vColor = aColor;
    vTexCoord = aTexCoord;
    gl_Position = push.uProjection * push.uModel * vec4(aPos, 1.0);
}

#[fragment]

#version 450
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec2 vTexCoord;

layout (location = 0) out vec4 FragColor;

void main() {
    FragColor = vColor;
}