#ifndef SCENE_H
#define SCENE_H

#include <map>
#include <memory>
#include <vector>

#include "src/camera.h"
#include "src/game_object.h"
#include "src/light_component.h"

class Scene {
 private:
  std::vector<std::unique_ptr<GameObject>> objects;
  std::vector<Camera> cameras;
  size_t activeCameraIdx;

  std::map<Material*, std::vector<GameObject*>> groupByMaterial();

  std::vector<LightComponent*> collectLights();

  void setLightUniforms(Shader* shader);

 public:
  Scene() : activeCameraIdx(0) {}

  void setActiveCamera(size_t index) { activeCameraIdx = index; }

  Camera& getActiveCamera() { return cameras[activeCameraIdx]; }

  void update(float deltaTime);

  void render();

  void addObject(std::unique_ptr<GameObject> obj) {
    objects.push_back(std::move(obj));
  }

  void addCamera(Camera cam) { cameras.push_back(cam); }

  std::vector<Camera>& getAllCameras() { return cameras; }
};

#endif /* SCENE_H */
