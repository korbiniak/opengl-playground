#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 baseColor;

void main()
{
  FragColor = vec4(baseColor * lightColor * lightIntensity, 1.0F);
}
