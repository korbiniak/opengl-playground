#ifndef DIRECTIONAL_LIGHT_COMPONENT_H
#define DIRECTIONAL_LIGHT_COMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/game_object.h"
#include "src/light_component.h"

class DirectionalLightComponent : public LightComponent {
 private:
  glm::vec3 direction;

  std::string getUniformArrayName() const override { return "dirLights"; }

 public:
  DirectionalLightComponent(glm::vec3 ambient, glm::vec3 diffuse,
                            glm::vec3 specular, glm::vec3 direction)
      : LightComponent(ambient, diffuse, specular),
        direction(glm::normalize(direction)) {}

  glm::vec3 getDirection() const {
    if (gameObject) {
      return glm::normalize(
          glm::vec3(glm::mat4_cast(gameObject->getWorldRotation()) *
                    glm::vec4(0, 0, -1, 0)));
    }
    return direction;
  }

  void setDirection(const glm::vec3& dir) { direction = glm::normalize(dir); }

  void setUniforms(Shader* shader, int idx) override {
    shader->setUniform(makeUniformName(idx, "direction"), getDirection());
    shader->setUniform(makeUniformName(idx, "ambient"), getAmbient());
    shader->setUniform(makeUniformName(idx, "diffuse"), getDiffuse());
    shader->setUniform(makeUniformName(idx, "specular"), getSpecular());
  }
};

#endif /* DIRECTIONAL_LIGHT_COMPONENT_H */
