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
  std::vector<std::unique_ptr<GameObject>> rootObjects;
  std::vector<Camera*> cameras;
  size_t activeCameraIdx;

  std::map<Material*, std::vector<GameObject*>> groupByMaterial();

  std::vector<LightComponent*> collectLights();

  void setLightUniforms(Shader* shader);

  template <typename Func>
  void forEachObject(Func func) {
    for (auto& root : rootObjects) {
      func(root.get());
      root->forEachChild(func);
    }
  }

 public:
  Scene() : activeCameraIdx(0) {}

  void setActiveCamera(size_t index) { activeCameraIdx = index; }

  Camera& getActiveCamera() { return *cameras[activeCameraIdx]; }

  void update(float deltaTime);

  void render();

  void addObject(std::unique_ptr<GameObject> obj) {
    rootObjects.push_back(std::move(obj));
  }

  void addCamera(std::unique_ptr<Camera> cam) {
    Camera* camPtr = cam.get();
    cameras.push_back(camPtr);
    rootObjects.push_back(std::move(cam));
  }

  GameObject* addChildToObject(GameObject* parent,
                               std::unique_ptr<GameObject> child) {
    return parent->addChild(std::move(child));
  }

  std::vector<Camera*>& getAllCameras() { return cameras; }

  /* Find maximum GameObject ID in scene (for post-deserialization) */
  uint64_t findMaxGameObjectId() const;

  /* Update GameObject::nextId to prevent ID collisions after loading */
  void updateNextGameObjectId();
};

#endif /* SCENE_H */
