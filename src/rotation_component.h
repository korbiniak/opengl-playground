#ifndef ROTATION_COMPONENT_H
#define ROTATION_COMPONENT_H

#include <glm/glm.hpp>

#include "src/component.h"

class RotationComponent : public Component {
 private:
  glm::vec3 axis;
  float angularSpeed;

 public:
  RotationComponent(const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f),
                    float angularSpeed = 45.0f);

  void update(float deltaTime) override;

  void setAxis(const glm::vec3& newAxis) { axis = glm::normalize(newAxis); }
  void setAngularSpeed(float speed) { angularSpeed = speed; }

  std::string getTypeName() const override { return "RotationComponent"; }
};

#endif /* ROTATION_COMPONENT_H */
