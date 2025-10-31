#include "src/utils.h"

#include <GL/glew.h>

#include "src/logger.h"

void checkGLError(const std::string& location) {
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    const char* error;
    switch (err) {
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
      default:
        error = "UNKNOWN";
        break;
    }
    LOG_ERROR("OpenGL Error at ", location, ": ", error, " (", err, ")");
  }
}

glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
  start = glm::normalize(start);
  dest = glm::normalize(dest);

  float cosTheta = glm::dot(start, dest);
  glm::vec3 rotationAxis;

  if (cosTheta < -0.999999f) {
    rotationAxis = glm::cross(glm::vec3(0, 0, 1), start);
    if (glm::length(rotationAxis) < 0.01f) {
      rotationAxis = glm::cross(glm::vec3(1, 0, 0), start);
    }
    rotationAxis = glm::normalize(rotationAxis);
    return glm::angleAxis(glm::radians(180.0f), rotationAxis);
  }

  rotationAxis = glm::cross(start, dest);
  float s = glm::sqrt((1 + cosTheta) * 2);
  float invs = 1 / s;

  return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs,
                   rotationAxis.z * invs);
}

unsigned int decodeUTF8(const char*& ptr) {
  unsigned char c = *ptr++;
  unsigned int result = 0;

  if (!(c & 0x80)) {
    result = c;
  } else if ((c & 0xE0) == 0xC0) {
    result = (c & 0x1F) << 6;
    result |= (*ptr++ & 0x3F);
  } else if ((c & 0xF0) == 0xE0) {
    result = (c & 0x0F) << 12;
    result |= (*ptr++ & 0x3F) << 6;
    result |= (*ptr++ & 0x3F);
  } else if ((c & 0xF8) == 0xF0) {
    result = (c & 0x07) << 18;
    result |= (*ptr++ & 0x3F) << 12;
    result |= (*ptr++ & 0x3F) << 6;
    result |= (*ptr++ & 0x3F);
  } else {
    LOG_WARNING("Weird UTF character, first byte: ", (unsigned int)c);
    result = '?';
  }

  return result;
}
