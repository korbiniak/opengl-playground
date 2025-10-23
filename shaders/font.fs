#version 330 core

in vec2 texCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
     float glyphShape = texture(text, texCoords).r;
     if (glyphShape < 0.5)
        discard;
     color = vec4(textColor, 1.0);
}