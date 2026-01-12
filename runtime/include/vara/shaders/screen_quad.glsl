#[vertex]

#version 410
layout (location = 0) out vec2 vTexCoord;

void main() {
    vec2 uv = vec2((gl_VertexID >> 1), gl_VertexID & 1) * 2.0;
    vec4 pos = vec4(uv * vec2(2, 2) + vec2(-1, -1), 0, 1);

    gl_Position = pos;
    vTexCoord = uv;
}

#[fragment]

#version 410
layout (location = 0) in vec2 vTexCoord;

layout (location = 0) out vec4 fragColor;

uniform sampler2D uScreenTexture;

void main() {
    fragColor = texture(uScreenTexture, vTexCoord);
}

