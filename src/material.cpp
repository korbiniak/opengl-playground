#include "src/material.h"

#include "src/logger.h"

Material::Material(std::shared_ptr<Shader> shader,
                   std::shared_ptr<Texture> texture, const glm::vec3& color)
    : shader(shader), texture(texture), baseColor(color) {}

void Material::bind() const {
  if (shader) {
    shader->use();
    shader->setUniform("baseColor", baseColor);

    if (texture) {
      texture->bind(0);
      // shader->setUniform("hasTexture", true);
    } else {
      // shader->setUniform("hasTexture", false);
    }
  }
}
