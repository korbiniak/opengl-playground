#include "src/game_object.h"

#include <typeindex>

#include "src/exceptions.h"
#include "src/utils.h"

GameObject::GameObject(std::shared_ptr<Mesh> mesh,
                       std::shared_ptr<Material> material)
    : mesh(std::move(mesh)),
      material(std::move(material)),
      position(0.0F),
      rotation(glm::quat(1.0F, 0.0F, 0.0F, 0.0F)),
      scale(1.0F),
      modelMatrix(1.0F),
      dirty(false) {
  assert(this->material);
}

void GameObject::updateModelMatrix() {
  glm::mat4 trans = glm::translate(glm::mat4(1.0F), position);
  glm::mat4 rot = glm::mat4_cast(rotation);
  glm::mat4 scl = glm::scale(glm::mat4(1.0F), scale);
  modelMatrix = trans * rot * scl;
}

const glm::mat4& GameObject::getModelMatrix() {
  if (dirty) {
    updateModelMatrix();
    dirty = false;
  }
  return modelMatrix;
}

void GameObject::drawGeometry() {
  material->getShader()->setUniform("model", getModelMatrix());
  if (mesh) {
    mesh->draw();
  }
}

void GameObject::update(float deltaTime) {
  for (std::unique_ptr<Component>& component : components) {
    if (component->isEnabled()) {
      component->update(deltaTime);
    }
  }

  if (dirty) {
    updateModelMatrix();
    dirty = false;
  }
}

void GameObject::faceDirection(const glm::vec3& targetDir) {
  glm::vec3 dir = glm::normalize(targetDir);
  glm::vec3 forward = glm::vec3(0, 0, -1);

  glm::quat rotation = rotationBetweenVectors(forward, dir);
  setRotation(rotation);
}
