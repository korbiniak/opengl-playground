#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include <memory>

#include <glm/glm.hpp>

#include "src/component.h"
#include "src/shader.h"

class LightComponent : public Component {
 protected:
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  virtual std::string getUniformArrayName() const = 0;

  std::string makeUniformName(int index, const std::string& property) const {
    return getUniformArrayName() + "[" + std::to_string(index) + "]." +
           property;
  }

 public:
  LightComponent(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
      : Component(), ambient(ambient), diffuse(diffuse), specular(specular) {}

  void onAttach(GameObject* obj) override {
    Component::onAttach(obj);
    if (gameObject && gameObject->getMaterial()) {
      gameObject->getMaterial()->setBaseColor(diffuse);
    }
  }

  void update(float deltaTime) override {}

  const glm::vec3& getAmbient() { return ambient; }
  const glm::vec3& getDiffuse() { return diffuse; }
  const glm::vec3& getSpecular() { return specular; }

  void setAmbient(const glm::vec3& a) { ambient = a; }
  void setDiffuse(const glm::vec3& a) { diffuse = a; }
  void setSpecular(const glm::vec3& a) { specular = a; }

  virtual void setUniforms(Shader* shader, int idx) = 0;
};

#endif /* LIGHT_COMPONENT_H */
