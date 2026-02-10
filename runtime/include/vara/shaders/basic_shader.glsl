#[vertex]

#version 450
layout (location = 0) in vec3 aPos;

layout (location = 0) out vec4 vColor;
layout (location = 1) out vec3 vWorldPos;

layout (set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
};

void main() {
    vec4 worldPos = vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vColor = vec4(0.5, 0.5, 0.5, 1.0);
    gl_Position = projection * view * worldPos;
}

#[fragment]

#version 450
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec3 vWorldPos;

layout (location = 0) out vec4 oColor;

void main() {
    oColor = vColor;
}