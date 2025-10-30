#include "src/rotation_component.h"

#include "src/game_object.h"
#include "src/logger.h"

RotationComponent::RotationComponent(const glm::vec3& axis, float angularSpeed)
    : Component(), axis(glm::normalize(axis)), angularSpeed(angularSpeed) {}

void RotationComponent::update(float deltaTime) {
  if (!gameObject) {
    LOG_WARNING("Update on detached component?");
    return;
  }

  float angleThisFrame = angularSpeed * deltaTime;
  gameObject->rotate(angleThisFrame, axis);
}
