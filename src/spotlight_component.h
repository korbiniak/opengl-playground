#ifndef SPOTLIGHT_COMPONENT_H
#define SPOTLIGHT_COMPONENT_H

#include <glm/glm.hpp>

#include "src/game_object.h"
#include "src/light_component.h"

class SpotlightComponent : public LightComponent {
 private:
  glm::vec3 direction;
  float cutOff;
  float outerCutOff;
  float constant;
  float linear;
  float quadratic;

  std::string getUniformArrayName() const override { return "spotLights"; }

 public:
  SpotlightComponent(const glm::vec3& direction, float cutOff = 12.5f,
                     float outerCutOff = 17.5f, float constant = 1.0f,
                     float linear = 0.09f, float quadratic = 0.032f,
                     glm::vec3 ambient = glm::vec3(0.1f),
                     glm::vec3 diffuse = glm::vec3(0.8f),
                     glm::vec3 specular = glm::vec3(1.0f))
      : LightComponent(ambient, diffuse, specular),
        direction(glm::normalize(direction)),
        cutOff(glm::cos(glm::radians(cutOff))),
        outerCutOff(glm::cos(glm::radians(outerCutOff))),
        constant(constant),
        linear(linear),
        quadratic(quadratic) {}

  glm::vec3 getDirection() const {
    if (gameObject) {
      return glm::normalize(
          glm::vec3(glm::mat4_cast(gameObject->getWorldRotation()) *
                    glm::vec4(0, 0, -1, 0)));
    }
    return direction;
  }

  void setDirection(const glm::vec3& dir) { direction = glm::normalize(dir); }

  float getCutOff() const { return cutOff; }
  float getOuterCutOff() const { return outerCutOff; }

  void setCutOff(float inner, float outer) {
    cutOff = glm::cos(glm::radians(inner));
    outerCutOff = glm::cos(glm::radians(outer));
  }

  float getConstant() const { return constant; }
  float getLinear() const { return linear; }
  float getQuadratic() const { return quadratic; }

  void setAttenuation(float c, float l, float q) {
    constant = c;
    linear = l;
    quadratic = q;
  }

  void setUniforms(Shader* shader, int idx) override {
    shader->setUniform(makeUniformName(idx, "position"),
                       gameObject->getWorldPosition());

    shader->setUniform(makeUniformName(idx, "direction"), getDirection());

    shader->setUniform(makeUniformName(idx, "cutOff"), getCutOff());
    shader->setUniform(makeUniformName(idx, "outerCutOff"), getOuterCutOff());
    shader->setUniform(makeUniformName(idx, "constant"), getConstant());
    shader->setUniform(makeUniformName(idx, "linear"), getLinear());
    shader->setUniform(makeUniformName(idx, "quadratic"), getQuadratic());

    shader->setUniform(makeUniformName(idx, "ambient"), getAmbient());
    shader->setUniform(makeUniformName(idx, "diffuse"), getDiffuse());
    shader->setUniform(makeUniformName(idx, "specular"), getSpecular());
  }
};

#endif /* SPOTLIGHT_COMPONENT_H */
