#[vertex]

#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexIndex;

layout (location = 0) out vec4 vColor;
layout (location = 1) out vec2 vTexCoord;
layout (location = 2) out float vTexIndex;

uniform mat4 uProjection;

void main() {
    vColor = aColor;
    vTexCoord = aTexCoord;
    vTexIndex = aTexIndex;
    gl_Position = uProjection * vec4(aPos, 1.0);
}

#[fragment]

#version 410
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in float vTexIndex;

layout (location = 0) out vec4 FragColor;

uniform sampler2D uTextures[16];

// Need to figure out font rendering.
// Maybe pass in flags?
void main() {
    int index = int(vTexIndex);
    vec4 texColor = texture(uTextures[index], vTexCoord);

    if (texColor.a <= 0.0f) {
        discard;
    }

    FragColor = texColor * vColor;
}