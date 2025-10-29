#include "src/camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "src/logger.h"
#include "src/utils.h"

Camera::Camera(glm::vec3 position, float aspectRatio, glm::vec3 front,
               glm::vec3 up, float speed, float fov)
    : GameObject(nullptr, nullptr),
      speed(speed),
      aspectRatio(aspectRatio),
      fov(fov),
      worldUp(glm::normalize(up)),
      yaw(0.0f),
      pitch(0.0f) {
  setPosition(position);

  glm::vec3 normFront = glm::normalize(front);
  yaw = glm::degrees(atan2f(normFront.x, -normFront.z));
  pitch = glm::degrees(asinf(normFront.y));

  glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
  glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
  setRotation(yawQuat * pitchQuat);

  updateMatrices();
}

glm::vec3 Camera::getFront() const {
  return glm::normalize(getRotation() * glm::vec3(0, 0, -1));
}

glm::vec3 Camera::getUp() const {
  return glm::normalize(getRotation() * glm::vec3(0, 1, 0));
}

glm::vec3 Camera::getRight() const {
  return glm::normalize(getRotation() * glm::vec3(1, 0, 0));
}

void Camera::updateMatrices() {
  glm::vec3 front = getFront();
  glm::vec3 up = getUp();
  LOG_DEBUG("Updating matrices: ", getPosition(), front, up);
  view = glm::lookAt(getPosition(), getPosition() + front, up);
  projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1F, 100.0F);
}

void Camera::update(float deltaTime) {
  GameObject::update(deltaTime);
  updateMatrices();
}

void Camera::rotate(float xoffset, float yoffset) {
  yaw -= xoffset;
  pitch += yoffset;

  pitch = glm::clamp(pitch, -89.0f, 89.0f);

  glm::quat yawQuat = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
  glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));

  setRotation(yawQuat * pitchQuat);
}

void Camera::moveForward(float deltaTime) {
  setPosition(getPosition() + getFront() * speed * deltaTime);
  updateMatrices();
}

void Camera::moveBackward(float deltaTime) {
  setPosition(getPosition() - getFront() * speed * deltaTime);
  updateMatrices();
}

void Camera::moveLeft(float deltaTime) {
  setPosition(getPosition() - getRight() * speed * deltaTime);
  updateMatrices();
}

void Camera::moveRight(float deltaTime) {
  setPosition(getPosition() + getRight() * speed * deltaTime);
  updateMatrices();
}

void Camera::moveUp(float deltaTime) {
  setPosition(getPosition() + worldUp * speed * deltaTime);
  updateMatrices();
}

void Camera::moveDown(float deltaTime) {
  setPosition(getPosition() - worldUp * speed * deltaTime);
  updateMatrices();
}
