#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>

#include "src/camera.h"
#include "src/game_object.h"

class Scene {
 private:
  std::vector<std::unique_ptr<GameObject>> objects;
  std::vector<Camera> cameras;
  size_t activeCameraIdx;

 public:
  Scene() : activeCameraIdx(0) {}

  void setActiveCamera(size_t index) { activeCameraIdx = index; }

  Camera& getActiveCamera() { return cameras[activeCameraIdx]; }

  void update(float deltaTime) {
    cameras[activeCameraIdx].updateMatrices();
    for (auto& obj : objects) {
      obj->update(deltaTime);
    }
  }

  void render(Shader& shader) {
    Camera& camera = getActiveCamera();
    shader.use();
    shader.setUniform("view", camera.getViewMatrix());
    shader.setUniform("projection", camera.getProjectionMatrix());

    for (auto& obj : objects) {
      obj->draw(shader);
    }
  }

  void addObject(std::unique_ptr<GameObject> obj) {
    objects.push_back(std::move(obj));
  }

  void addCamera(Camera cam) { cameras.push_back(cam); }
};

#endif /* SCENE_H */
