#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include <glm/glm.hpp>

#include "src/logger.h"
#include "src/shader.h"
#include "src/texture.h"

class Material {
 private:
  /* TODO: in the future the material could store a list of pairs to know what uniforms to bind */
  std::shared_ptr<Shader> shader;
  std::shared_ptr<Texture> texture;
  std::shared_ptr<Texture> specular;

  float shininess;

  glm::vec3 baseColor;

 public:
  Material(std::shared_ptr<Shader> shader,
           std::shared_ptr<Texture> texture = nullptr,
           std::shared_ptr<Texture> specular = nullptr, float shininess = 32,
           glm::vec3 baseColor = glm::vec3(1.0F));

  void bind() const;

  std::shared_ptr<Shader> getShader() const { return shader; }
  std::shared_ptr<Texture> getTexture() const { return texture; }

  void setShader(std::shared_ptr<Shader> s) { shader = std::move(s); }
  void setTexture(std::shared_ptr<Texture> tex) { texture = std::move(tex); }
  void setBaseColor(const glm::vec3& color) { baseColor = color; }

  const glm::vec3& getBaseColor() const { return baseColor; }
};

#endif /* MATERIAL_H */
