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
      localMatrix(1.0F),
      modelMatrix(1.0F),
      dirty(true),
      parent(nullptr) {}

void GameObject::markDirty() {
  dirty = true;
  // Propagate to all children
  for (auto& child : children) {
    child->markDirty();
  }
}

void GameObject::updateModelMatrix() const {
  glm::mat4 trans = glm::translate(glm::mat4(1.0F), position);
  glm::mat4 rot = glm::mat4_cast(rotation);
  glm::mat4 scl = glm::scale(glm::mat4(1.0F), scale);
  localMatrix = trans * rot * scl;

  if (parent) {
    modelMatrix = parent->getModelMatrix() * localMatrix;
  } else {
    modelMatrix = localMatrix;
  }
}

const glm::mat4& GameObject::getModelMatrix() const {
  if (dirty) {
    updateModelMatrix();
    dirty = false;
  }
  return modelMatrix;
}

void GameObject::drawGeometry() {
  if (!material) {
    return;
  }
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

GameObject* GameObject::addChild(std::unique_ptr<GameObject> child) {
  GameObject* childPtr = child.get();
  child->parent = this;
  child->markDirty();
  children.push_back(std::move(child));
  return childPtr;
}

std::unique_ptr<GameObject> GameObject::removeChild(GameObject* child) {
  for (auto it = children.begin(); it != children.end(); ++it) {
    if (it->get() == child) {
      std::unique_ptr<GameObject> removed = std::move(*it);
      children.erase(it);
      removed->parent = nullptr;
      removed->markDirty();
      return removed;
    }
  }
  return nullptr;
}

void GameObject::setParent(GameObject* newParent) {
  /* Note: This method should typically not be called directly
     Use Scene reparenting methods or manually transfer ownership
     This just updates the parent pointer */
  if (parent == newParent) {
    return;
  }

  parent = newParent;
  markDirty();
}

glm::vec3 GameObject::getWorldPosition() const {
  return glm::vec3(getModelMatrix()[3]);
}

glm::quat GameObject::getWorldRotation() const {
  if (parent) {
    return parent->getWorldRotation() * rotation;
  }
  return rotation;
}

glm::vec3 GameObject::getWorldScale() const {
  const glm::mat4& worldMat = getModelMatrix();
  glm::vec3 worldScale;
  worldScale.x = glm::length(glm::vec3(worldMat[0]));
  worldScale.y = glm::length(glm::vec3(worldMat[1]));
  worldScale.z = glm::length(glm::vec3(worldMat[2]));
  return worldScale;
}

glm::mat4 GameObject::getWorldMatrix() const {
  return getModelMatrix();
}
