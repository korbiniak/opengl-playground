#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "src/shader.h"

class GameObject {
 protected:
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;
  glm::mat4 modelMatrix;
  bool dirty;

  void updateModelMatrix();

 public:
  GameObject();

  void update(float deltaTime) {
    (void)deltaTime;
    updateModelMatrix();
  }

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

  virtual void draw(Shader& shader) = 0;
};

#endif /* GAME_OBJECT_H */
