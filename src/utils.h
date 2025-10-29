#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

extern void checkGLError(const std::string& location);

extern glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

#endif /* UTILS_H */
