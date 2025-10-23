#include "src/game_object.h"

GameObject::GameObject()
    : position(0.0F),
      rotation(glm::quat(1.0F, 0.0F, 0.0F, 0.0F)),
      scale(1.0F),
      modelMatrix(1.0F),
      dirty(false) {}

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
