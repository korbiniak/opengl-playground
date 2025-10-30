#version 330 core

in vec2 texCoords;
out vec4 color;

uniform sampler2D fontAtlas;
uniform vec3 textColor;

void main() {
    float distance = texture(fontAtlas, texCoords).r;

    float smoothWidth = fwidth(distance) * 0.5;
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);

    color = vec4(textColor, alpha);
}
