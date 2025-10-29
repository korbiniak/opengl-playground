#version 330 core
out vec4 FragColor;

struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

struct SpotLight {
  vec3 position;

  vec3 direction;
  float cutOff;
  float outerCutOff;
  float constant;
  float linear;
  float quadratic;


  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

#define MAX_POINT_LIGHTS 8
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

#define MAX_DIR_LIGHTS 8
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform int numDirLights;

#define MAX_SPOT_LIGHTS 8
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int numSpotLights;

uniform Material material;

uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

float specularStrength = 0.5;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * vec3(texture(material.specular, TexCoords));

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon   = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = spec * light.specular * vec3(texture(material.specular, TexCoords));

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  return attenuation * (ambient + intensity * (diffuse + specular));
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = spec * light.specular * vec3(texture(material.specular, TexCoords));

  return ambient + diffuse + specular;
}

void main()
{
  vec3 result = vec3(0.0);
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  for (int i = 0; i < numPointLights; ++i) {
    result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
  }
  for (int i = 0; i < numSpotLights; ++i) {
    result += CalcSpotLight(spotLights[i], normal, FragPos, viewDir);
  }
  for (int i = 0; i < numDirLights; ++i) {
    result += CalcDirLight(dirLights[i], normal, viewDir);
  }

  FragColor = vec4(result, 1.0);
}
