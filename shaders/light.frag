#version 330 core
out vec4 FragColor;

struct Light {
  vec3 position;
  vec3 color;
  float intensity;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 baseColor;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

float specularStrength = 0.5;

void main()
{
  float ambientStrength = 0.1;

  vec3 result = vec3(0.0);
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  for (int i = 0; i < numLights; ++i) {
    vec3 ambient = ambientStrength * lights[i].color * lights[i].intensity;

    vec3 lightDir = normalize(lights[i].position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lights[i].color * lights[i].intensity;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lights[i].color;

    result += baseColor * (ambient + diffuse + specular);
  }

  FragColor = vec4(result, 1.0);
}
