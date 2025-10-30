#version 330 core

in vec2 TexCoords;

out vec4 color;

struct Material {
  sampler2D diffuse;
};

uniform Material material;
uniform vec3 baseColor;

void main() {
    float distance = texture(material.diffuse, TexCoords).r;

    float smoothWidth = fwidth(distance) * 0.5;
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);

    color = vec4(baseColor, alpha);
}
