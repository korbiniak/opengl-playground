#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// #include "light_incl.frag"

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

uniform Material material;

void main()
{
  vec3 result = vec3(0.0);
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 diffuseColor = vec3(texture(material.diffuse, TexCoords));
  vec3 specularColor = vec3(texture(material.specular, TexCoords));
  float shininess = material.shininess;

  for (int i = 0; i < numPointLights; ++i) {
    result += CalcPointLight(pointLights[i], normal, FragPos, viewDir, diffuseColor, specularColor, shininess);
  }
  for (int i = 0; i < numSpotLights; ++i) {
    result += CalcSpotLight(spotLights[i], normal, FragPos, viewDir, diffuseColor, specularColor, shininess);
  }
  for (int i = 0; i < numDirLights; ++i) {
    result += CalcDirLight(dirLights[i], normal, viewDir, diffuseColor, specularColor, shininess);
  }

  FragColor = vec4(result, 1.0);
}
