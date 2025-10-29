#ifndef POINT_LIGHT_COMPONENT_H
#define POINT_LIGHT_COMPONENT_H

#include <glm/glm.hpp>

#include "src/game_object.h"
#include "src/light_component.h"

class PointLightComponent : public LightComponent {
 private:
  glm::vec3 direction;

  float constant;
  float linear;
  float quadratic;

  std::string getUniformArrayName() const override { return "pointLights"; }

 public:
  PointLightComponent(float constant = 1.0f, float linear = 0.09f,
                      float quadratic = 0.032f,
                      glm::vec3 ambient = glm::vec3(0.1f),
                      glm::vec3 diffuse = glm::vec3(0.8f),
                      glm::vec3 specular = glm::vec3(1.0f))
      : LightComponent(ambient, diffuse, specular),
        constant(constant),
        linear(linear),
        quadratic(quadratic) {}

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

    shader->setUniform(makeUniformName(idx, "constant"), getConstant());
    shader->setUniform(makeUniformName(idx, "linear"), getLinear());
    shader->setUniform(makeUniformName(idx, "quadratic"), getQuadratic());

    shader->setUniform(makeUniformName(idx, "ambient"), getAmbient());
    shader->setUniform(makeUniformName(idx, "diffuse"), getDiffuse());
    shader->setUniform(makeUniformName(idx, "specular"), getSpecular());
  }
};

#endif /* POINT_LIGHT_COMPONENT_H */
