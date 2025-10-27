#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <memory>

#include "glm/glm.hpp"

#include "src/game_object.h"
#include "src/mesh.h"

class LightSource : public GameObject {
 private:
  glm::vec3 color;
  float intensity;

 public:
  LightSource(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
              glm::vec3 color = glm::vec3(1.0F), float intensity = 1.0F);

  void drawGeometry() override;

  const glm::vec3& getColor() { return color; }
  float getIntensity() { return intensity; }
};

#endif /* LIGHT_SOURCE_H */
