#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

extern void checkGLError(const std::string& location);

extern glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

extern unsigned int decodeUTF8(const char*& ptr);

#endif /* UTILS_H */
