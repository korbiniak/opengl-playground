#include "src/circular_motion_component.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/game_object.h"
#include "src/logger.h"

CircularMotionComponent::CircularMotionComponent(const glm::vec3& center,
                                                 float radius, float speed,
                                                 const glm::vec3& axis)
    : center(center),
      radius(radius),
      speed(speed),
      currentAngle(0.0f),
      axis(glm::normalize(axis)) {}

void CircularMotionComponent::update(float deltaTime) {
  if (!gameObject) {
    LOG_WARNING("Update on detached component?");
  }

  constexpr float fullRotation = 2.0f * glm::pi<float>();

  LOG_DEBUG(currentAngle);

  currentAngle += speed * deltaTime;
  while (currentAngle > fullRotation) {
    currentAngle -= fullRotation;
  }

  glm::vec3 offset;
  if (axis == glm::vec3(0.0F, 1.0F, 0.0F)) {
    offset = glm::vec3(radius * glm::cos(currentAngle), 0.0F,
                       radius * glm::sin(currentAngle));
  } else {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0F), currentAngle, axis);
    offset = glm::vec3(rotation * glm::vec4(0.0F, radius, 0.0F, 1.0F));
  }

  gameObject->setPosition(center + offset);
};
