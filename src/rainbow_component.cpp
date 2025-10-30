#include "src/rainbow_component.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/color_space.hpp>

#include "src/game_object.h"
#include "src/logger.h"

RainbowComponent::RainbowComponent(float speed)
    : Component(), hue(0.0f), speed(speed) {}

void RainbowComponent::update(float deltaTime) {
  if (!gameObject) {
    LOG_WARNING("Update on detached component?");
  }

  hue += deltaTime * speed;
  if (hue > 360.0f) {
    hue -= 360.0f;
  }

  glm::vec3 hsvColor(hue, 1.0f, 1.0f);
  glm::vec3 rgbColor = glm::rgbColor(hsvColor);

  gameObject->getMaterial()->setBaseColor(rgbColor);
}
