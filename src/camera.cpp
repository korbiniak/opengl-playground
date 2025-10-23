#include "src/camera.h"

#include "src/logger.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float speed,
               float aspectRatio, float yaw, float pitch, float fov)
    : position(position),
      front(front),
      up(up),
      right(glm::cross(front, up)),
      speed(speed),
      yaw(yaw),
      pitch(pitch),
      aspectRatio(aspectRatio),
      fov(fov) {
  // updateVectors();
  updateMatrices();
}

void Camera::updateVectors() {
  glm::vec3 direction;
  direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
  direction.y = sinf(glm::radians(pitch));
  direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

  front = glm::normalize(direction);
  right = glm::cross(front, up);

  updateMatrices();
}

void Camera::updateMatrices() {
  LOG_DEBUG("Updating matrices: ", position, front, up, right);
  view = glm::lookAt(position, position + front, up);
  projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1F, 100.0F);
}

void Camera::rotate(float xoffset, float yoffset) {
  yaw += xoffset;
  pitch += yoffset;
  pitch = glm::clamp(pitch, -89.0F, 89.0F);
  updateVectors();
}

void Camera::moveForward(float deltaTime) {
  position += front * speed * deltaTime;
  updateMatrices();
}
void Camera::moveBackward(float deltaTime) {
  position -= front * speed * deltaTime;
  updateMatrices();
}
void Camera::moveLeft(float deltaTime) {
  position -= right * speed * deltaTime;
  updateMatrices();
}
void Camera::moveRight(float deltaTime) {
  position += right * speed * deltaTime;
  updateMatrices();
}
void Camera::moveUp(float deltaTime) {
  position += up * speed * deltaTime;
  updateMatrices();
}
void Camera::moveDown(float deltaTime) {
  position -= up * speed * deltaTime;
  updateMatrices();
}
