#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// #include "light_incl.frag"

struct Material {
  sampler2D diffuse;
};

uniform Material material;
uniform vec3 baseColor;

void main()
{
  vec3 result = vec3(0.0);
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  float distance = texture(material.diffuse, TexCoords).r;
  float smoothWidth = fwidth(distance) * 0.5;
  float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);

  vec3 diffuseColor = baseColor;
  vec3 specularColor = vec3(1.0);
  float shininess = 32.0;

  for (int i = 0; i < numPointLights; ++i) {
    result += CalcPointLight(pointLights[i], normal, FragPos, viewDir, diffuseColor, specularColor, shininess);
  }
  for (int i = 0; i < numSpotLights; ++i) {
    result += CalcSpotLight(spotLights[i], normal, FragPos, viewDir, diffuseColor, specularColor, shininess);
  }
  for (int i = 0; i < numDirLights; ++i) {
    result += CalcDirLight(dirLights[i], normal, viewDir, diffuseColor, specularColor, shininess);
  }

  FragColor = vec4(result, alpha);
}
