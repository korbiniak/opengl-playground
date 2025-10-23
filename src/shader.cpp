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

#include "src/exceptions.h"
#include "src/logger.h"
#include "src/utils.h"

std::string readSourceFile(const std::filesystem::path& path) {
  std::string code;
  std::ifstream sourceFile;

  LOG_INFO("Opening shader", path);

  sourceFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    sourceFile.open(path);
    std::stringstream shaderStream;
    shaderStream << sourceFile.rdbuf();
    sourceFile.close();
    code = shaderStream.str();
  } catch (std::ifstream::failure e) {
    LOG_ERROR("Cannot open shader under ", path, ": ", strerror(errno));
    throw;
  }

  return code;
}

GLuint compileShader(const std::string& source, GLuint shaderType) {
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
    LOG_ERROR("Shader compilation failed: ", infoLog);
    throw ShaderCompilationException(infoLog);
  }

  return shader;
}

GLuint linkProgram(unsigned int vertexShader, unsigned int fragmentShader) {
  int success;
  char infoLog[512];

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    LOG_ERROR("Shader linking failed: ", infoLog);
    throw ShaderCompilationException(infoLog);
  }

  return shaderProgram;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
  std::string vShaderCode = readSourceFile(vertexPath);
  std::string fShaderCode = readSourceFile(fragmentPath);
  GLuint vertexShader = compileShader(vShaderCode, GL_VERTEX_SHADER);
  GLuint fragmentShader = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
  GLuint shaderProgram = linkProgram(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  id = shaderProgram;
}

Shader::~Shader() {
  glDeleteProgram(id);
}

void Shader::use() {
  glUseProgram(id);

  checkGLError("after shader.use()");
}

void Shader::setUniform(const std::string& name, int val) {
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  if (currentProgram != id) {
    LOG_ERROR("Shader ", id, " not active when setting '", name, "'");
    return;
  }
  if (glGetUniformLocation(id, name.c_str()) == -1) {
    LOG_WARNING("Can't find uniform " + name);
  }

  glUniform1i(glGetUniformLocation(id, name.c_str()), val);
  checkGLError("after setUniform(int) for name " + name);
}

void Shader::setUniform(const std::string& name, const glm::mat4& mat4) {
  if (glGetUniformLocation(id, name.c_str()) == -1) {
    LOG_WARNING("Can't find uniform " + name);
  }
  glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(mat4));
  checkGLError("after setUniform(mat4) for name " + name);
}

void Shader::setUniform(const std::string& name, const glm::vec3& vec3) {
  if (glGetUniformLocation(id, name.c_str()) == -1) {
    LOG_WARNING("Can't find uniform " + name);
  }
  glUniform3f(glGetUniformLocation(id, name.c_str()), vec3.x, vec3.y, vec3.z);
  checkGLError("after setUniform(vec3) for name " + name);
}
