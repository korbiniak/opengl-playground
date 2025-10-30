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

#define MAX_POINT_LIGHTS 8
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

#define MAX_DIR_LIGHTS 8
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform int numDirLights;

#define MAX_SPOT_LIGHTS 8
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int numSpotLights;

uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos,
                    vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float shininess) {
    vec3 ambient = light.ambient * diffuseColor;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * diffuseColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * light.specular * specularColor;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,
                   vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float shininess) {
  vec3 lightDir = normalize(light.position - fragPos);
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon   = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = diff * light.diffuse * diffuseColor;

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specular = spec * light.specular * specularColor;

  vec3 ambient = light.ambient * diffuseColor;

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  return attenuation * (ambient + intensity * (diffuse + specular));
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,
                  vec3 diffuseColor, vec3 specularColor, float shininess) {
  vec3 lightDir = normalize(-light.direction);

  vec3 ambient = light.ambient * diffuseColor;

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = diff * light.diffuse * diffuseColor;

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specular = spec * light.specular * specularColor;

  return ambient + diffuse + specular;
}
