#include "src/material.h"

#include "src/logger.h"

Material::Material(std::shared_ptr<Shader> shader,
                   std::shared_ptr<Texture> texture,
                   std::shared_ptr<Texture> specular, float shininess,
                   glm::vec3 baseColor, bool opaque)
    : shader(std::move(shader)),
      texture(std::move(texture)),
      specular(std::move(specular)),
      shininess(shininess),
      baseColor(baseColor),
      opaque(opaque) {}

void Material::bind() const {
  if (shader) {
    shader->use();

    if (shader->hasUniform("material.shininess")) {
      shader->setUniform("material.shininess", shininess);
    }

    if (shader->hasUniform("baseColor")) {
      shader->setUniform("baseColor", baseColor);
    }
    if (texture) {
      texture->bind(0);
      shader->setUniform("material.diffuse", 0);
    }
    if (specular) {
      specular->bind(1);
      shader->setUniform("material.specular", 1);
    }
  }
}
