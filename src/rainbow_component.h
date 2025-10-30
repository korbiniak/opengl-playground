#ifndef RAINBOW_COMPONENT_H
#define RAINBOW_COMPONENT_H

#include <glm/glm.hpp>

#include "src/component.h"

class RainbowComponent : public Component {
 private:
  float hue;
  float speed;

 public:
  RainbowComponent(float speed);

  void update(float deltaTime) override;

  std::string getTypeName() const override { return "RainbowComponent"; }
};

#endif /* RAINBOW_COMPONENT_H */
