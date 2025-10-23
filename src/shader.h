#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>

class Shader {
 public:
  unsigned int id;

  Shader(const std::string& vertexPath, const std::string& fragmentPath);
  void use();
  void setInt(const std::string& name, int val);
  void setMat4(const std::string& name, const glm::mat4& mat4);
  void setVec3(const std::string& name, const glm::vec3& vec3);
};

#endif /* SHADER_H */
