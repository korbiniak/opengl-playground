#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>

class Shader {
 public:
  GLint id;

  Shader(const std::string& vertexPath, const std::string& fragmentPath);
  void use();
  void setUniform(const std::string& name, int val);
  void setUniform(const std::string& name, float val);
  void setUniform(const std::string& name, const glm::mat4& mat4);
  void setUniform(const std::string& name, const glm::vec3& vec3);
  bool hasUniform(const std::string& name) const;

  ~Shader();
};

#endif /* SHADER_H */
