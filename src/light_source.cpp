#include "light_source.h"

LightSource::LightSource(std::shared_ptr<Mesh> mesh,
                         std::shared_ptr<Material> material, glm::vec3 color,
                         float intensity)
    : GameObject(std::move(mesh), std::move(material)),
      color(color),
      intensity(intensity) {
  assert(this->material);
}

void LightSource::drawGeometry() {
  Shader* shader = material->getShader().get();

  shader->setUniform("lightColor", color);
  shader->setUniform("lightIntensity", intensity);

  material->getShader()->setUniform("model", getModelMatrix());
  if (mesh) {
    mesh->draw();
  }
}
