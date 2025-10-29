#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <typeindex>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "src/component.h"
#include "src/exceptions.h"
#include "src/material.h"
#include "src/mesh.h"
#include "src/shader.h"

class GameObject {
 protected:
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  std::vector<std::unique_ptr<Component>> components;

  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;
  glm::mat4 modelMatrix;
  bool dirty;

  void updateModelMatrix();

 public:
  GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

  template <typename T>
  T* addComponent(std::unique_ptr<T> component) {
    static_assert(std::is_base_of<Component, T>::value,
                  "T must inherit from Component");

    T* ptr = component.get();
    component->onAttach(this);
    components.push_back(std::move(component));
    return ptr;
  }

  template <typename T>
  std::vector<T*> getComponents() {
    std::vector<T*> comps;
    for (std::unique_ptr<Component>& component : components) {
      if (T* casted = dynamic_cast<T*>(component.get())) {
        comps.push_back(casted);
      }
    }
    return comps;
  }

  template <typename T>
  void removeComponent() {
    throw NotImplementedException("");
  }

  template <typename T>
  T* hasComponent() {
    throw NotImplementedException("");
  }

  void update(float deltaTime);

  void setPosition(const glm::vec3& pos) {
    position = pos;
    dirty = true;
  }
  void setRotation(const glm::quat& rot) {
    rotation = rot;
    dirty = true;
  }
  void setScale(const glm::vec3& s) {
    scale = s;
    dirty = true;
  }

  void translate(const glm::vec3& offset) {
    position += offset;
    dirty = true;
  }
  void rotate(float angle, const glm::vec3& axis) {
    rotation =
        glm::angleAxis(glm::radians(angle), glm::normalize(axis)) * rotation;
    dirty = true;
  }

  const glm::vec3& getPosition() const { return position; }
  const glm::quat& getRotation() const { return rotation; }
  const glm::vec3& getScale() const { return scale; }
  const glm::mat4& getModelMatrix();
  const std::shared_ptr<Material> getMaterial() const { return material; }

  void faceDirection(const glm::vec3& targetDir);

  /* Assumes material is already bound */
  virtual void drawGeometry();
};

#endif /* GAME_OBJECT_H */
