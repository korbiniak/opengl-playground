#ifndef CIRCULAR_MOTION_COMPONENT_H
#define CIRCULAR_MOTION_COMPONENT_H

#include <glm/glm.hpp>

#include "src/component.h"

class CircularMotionComponent : public Component {
 private:
  glm::vec3 center;
  float radius;
  float speed;
  float currentAngle;
  glm::vec3 axis;

 public:
  CircularMotionComponent(const glm::vec3& center, float radius, float speed,
                          const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f));

  void update(float deltaTime) override;

  void setCenter(const glm::vec3& c) { center = c; }
  void setRadius(float r) { radius = r; }
  void setSpeed(float s) { speed = s; }
  void setAxis(const glm::vec3& a) { axis = glm::normalize(a); }

  float getCurrentAngle() const { return currentAngle; }
};

#endif /* CIRCULAR_MOTION_COMPONENT_H */
