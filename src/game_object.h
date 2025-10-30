#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <cstdint>
#include <string>
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
 private:
  static uint64_t nextId;
  static uint64_t generateId() { return nextId++; }

 protected:
  uint64_t id;
  std::string name;

  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  std::vector<std::unique_ptr<Component>> components;

  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;
  mutable glm::mat4 localMatrix;
  mutable glm::mat4 modelMatrix;
  mutable bool dirty;

  GameObject* parent;
  std::vector<std::unique_ptr<GameObject>> children;

  void updateModelMatrix() const;
  void markDirty();

 public:
  GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

  uint64_t getId() const { return id; }
  const std::string& getName() const { return name; }
  void setName(const std::string& n) { name = n; }

  /* For deserialization - set ID directly without generating */
  void setId(uint64_t newId) { id = newId; }

  /* Called after loading a scene to prevent ID collisions */
  static void setNextId(uint64_t nextIdValue) { nextId = nextIdValue; }

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

  virtual void update(float deltaTime);

  void setPosition(const glm::vec3& pos) {
    position = pos;
    markDirty();
  }
  void setRotation(const glm::quat& rot) {
    rotation = rot;
    markDirty();
  }
  void setScale(const glm::vec3& s) {
    scale = s;
    markDirty();
  }

  void translate(const glm::vec3& offset) {
    position += offset;
    markDirty();
  }
  void rotate(float angle, const glm::vec3& axis) {
    rotation =
        glm::angleAxis(glm::radians(angle), glm::normalize(axis)) * rotation;
    markDirty();
  }

  const glm::vec3& getPosition() const { return position; }
  const glm::quat& getRotation() const { return rotation; }
  const glm::vec3& getScale() const { return scale; }
  const glm::mat4& getModelMatrix() const;
  const std::shared_ptr<Material> getMaterial() const { return material; }

  void faceDirection(const glm::vec3& targetDir);

  void setParent(GameObject* newParent);
  GameObject* addChild(std::unique_ptr<GameObject> child);
  std::unique_ptr<GameObject> removeChild(GameObject* child);
  GameObject* getParent() const { return parent; }
  const std::vector<std::unique_ptr<GameObject>>& getChildren() const {
    return children;
  }

  glm::vec3 getWorldPosition() const;
  glm::quat getWorldRotation() const;
  glm::vec3 getWorldScale() const;
  glm::mat4 getWorldMatrix() const;

  template <typename Func>
  void forEachChild(Func func) {
    for (auto& child : children) {
      func(child.get());
      child->forEachChild(func);
    }
  }

  /* Assumes material is already bound */
  virtual void drawGeometry();
};

#endif /* GAME_OBJECT_H */
