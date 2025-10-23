#include "src/shader.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string readSourceFile(const std::filesystem::path& path) {
  std::string code;
  std::ifstream sourceFile;

  std::cout << "INFO::SHADER::COMPILATION: trying to open shader under " << path
            << std::endl;

  sourceFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    sourceFile.open(path);
    std::stringstream shaderStream;
    shaderStream << sourceFile.rdbuf();
    sourceFile.close();
    code = shaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_READ_ERROR: " << strerror(errno)
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return code;
}

unsigned int createShader(const std::string& source, GLuint shaderType) {
  int success;
  char infoLog[512];
  std::string shaderTypeString =
      shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";

  unsigned int shader = glCreateShader(shaderType);
  const char* str = source.c_str();

  glShaderSource(shader, 1, &str, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderTypeString
              << "::COMPILATION_FAILED: " << infoLog << std::endl;
    exit(EXIT_FAILURE);
  }

  return shader;
}

unsigned int createProgram(unsigned int vertexShader,
                           unsigned int fragmentShader) {
  int success;
  char infoLog[512];

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED: " << infoLog
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return shaderProgram;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
  std::string vShaderCode = readSourceFile(vertexPath);
  std::string fShaderCode = readSourceFile(fragmentPath);
  unsigned int vertexShader = createShader(vShaderCode, GL_VERTEX_SHADER);
  unsigned int fragmentShader = createShader(fShaderCode, GL_FRAGMENT_SHADER);
  unsigned int shaderProgram = createProgram(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  id = shaderProgram;
}

void Shader::use() {
  glUseProgram(id);
}

void Shader::setInt(const std::string& name, int val) {
  glUniform1i(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat4) {
  glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(mat4));
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec3) {
  glUniform3f(glGetUniformLocation(id, name.c_str()), vec3.x, vec3.y, vec3.z);
}
