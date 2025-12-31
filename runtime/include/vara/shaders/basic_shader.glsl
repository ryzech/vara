#[vertex]

#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec3 vWorldPos;
out vec2 vTexCoord;

uniform mat4 uViewProj;
uniform mat4 uModel;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vColor = aColor;
    vTexCoord = aTexCoord;
    gl_Position = uViewProj * worldPos;
}

#[fragment]

#version 410
layout(location = 0) out vec4 oColor;

in vec4 vColor;
in vec3 vWorldPos;
in vec2 vTexCoord;

uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, vTexCoord);

    if (texColor.a <= 0.0f) {
        discard;
    }

    oColor = texColor * vColor;
}