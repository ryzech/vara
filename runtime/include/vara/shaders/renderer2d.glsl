#[vertex]

#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexIndex;

out vec3 vColor;
out vec2 vTexCoord;
out float vTexIndex;

uniform mat4 uProjection;

void main() {
    vColor = aColor;
    vTexCoord = aTexCoord;
    vTexIndex = aTexIndex;
    gl_Position = uProjection * vec4(aPos, 1.0);
}

#[fragment]

#version 410
in vec3 vColor;
in vec2 vTexCoord;
in float vTexIndex;

out vec4 FragColor;

uniform sampler2D uTextures[16];

void main() {
    int index = int(vTexIndex);
    vec4 texColor = texture(uTextures[index], vTexCoord);
    FragColor = texColor * vec4(vColor, 1.0);
}