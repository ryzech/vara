#[vertex]

#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec4 vColor;
layout (location = 1) out vec3 vWorldPos;
layout (location = 2) out vec2 vTexCoord;

layout (std140) uniform CameraUBO {
    mat4 view;
    mat4 projection;
};

void main() {
    vec4 worldPos = vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vColor = aColor;
    vTexCoord = aTexCoord;
    gl_Position = projection * view * worldPos;
}

#[fragment]

#version 410
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec3 vWorldPos;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec4 oColor;

void main() {
    oColor = vColor;
}