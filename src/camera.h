#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* Camera class. No roll, only yaw and pitch. */
class Camera {
 private:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;

  float speed;
  float yaw;
  float pitch;
  float aspectRatio;
  float fov;

  glm::mat4 view;
  glm::mat4 projection;

  void updateVectors();

 public:
  Camera(glm::vec3 position, glm::vec3 front = glm::vec3(0.0F, 0.0F, -1.0F),
         glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F), float speed = 2.5F,
         float aspectRatio = 800.0F / 600.0F, float yaw = -90.0F,
         float pitch = 0.0F, float fov = 45.0F);

  void updateMatrices();

  void rotate(float xoffset, float yoffset);

  void moveForward(float deltaTime);
  void moveBackward(float deltaTime);
  void moveLeft(float deltaTime);
  void moveRight(float deltaTime);
  void moveUp(float deltaTime);
  void moveDown(float deltaTime);

  const glm::mat4& getViewMatrix() { return view; }
  const glm::mat4& getProjectionMatrix() { return projection; }
  const glm::vec3& getPosition() { return position; }
};

#endif /* CAMERA_H */
