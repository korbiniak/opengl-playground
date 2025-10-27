#include "src/scene.h"

#include <memory>

#include "src/game_object.h"

void Scene::update(float deltaTime) {
  cameras[activeCameraIdx].updateMatrices();

  for (auto& obj : objects) {
    obj->update(deltaTime);
  }
  for (auto& obj : lightSources) {
    obj->update(deltaTime);
  }
}

std::map<Material*, std::vector<GameObject*>> Scene::groupByMaterial() {
  std::map<Material*, std::vector<GameObject*>> groups;

  for (auto& obj : objects) {
    Material* mat = obj->getMaterial().get();
    if (mat) {
      groups[mat].push_back(obj.get());
    }
  }

  for (auto& light : lightSources) {
    Material* mat = light->getMaterial().get();
    if (mat) {
      groups[mat].push_back(light.get());
    }
  }

  return groups;
}

void Scene::setLightUniforms(Shader* shader) {
  shader->setUniform("numLights", static_cast<int>(lightSources.size()));

  for (size_t i = 0; i < lightSources.size(); i++) {
    std::string base = "lights[" + std::to_string(i) + "]";
    shader->setUniform(base + ".position", lightSources[i]->getPosition());
    shader->setUniform(base + ".color", lightSources[i]->getColor());
    shader->setUniform(base + ".intensity", lightSources[i]->getIntensity());
  }
}

bool needsLightning(Shader* shader) {
  return shader->hasUniform("numLights");
}

bool needsCameraPosition(Shader* shader) {
  return shader->hasUniform("viewPos");
}

void Scene::render() {
  Camera& camera = getActiveCamera();

  const glm::mat4& viewMatrix = camera.getViewMatrix();
  const glm::mat4& projectionMatrix = camera.getProjectionMatrix();

  std::map<Material*, std::vector<GameObject*>> groups = groupByMaterial();

  for (auto [material, group] : groups) {
    Shader* shader = material->getShader().get();
    material->bind();
    shader->setUniform("view", viewMatrix);
    shader->setUniform("projection", projectionMatrix);

    if (needsLightning(shader)) {
      setLightUniforms(shader);
    }
    if (needsCameraPosition(shader)) {
      shader->setUniform("viewPos", camera.getPosition());
    }
    for (GameObject* obj : group) {
      obj->drawGeometry();
    }
  }
}
