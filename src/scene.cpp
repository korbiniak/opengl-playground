#include "src/scene.h"

#include <memory>

#include "src/directional_light_component.h"
#include "src/game_object.h"
#include "src/light_component.h"
#include "src/point_light_component.h"
#include "src/spotlight_component.h"

void Scene::update(float deltaTime) {
  cameras[activeCameraIdx].updateMatrices();

  for (auto& obj : objects) {
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

  return groups;
}

std::vector<LightComponent*> Scene::collectLights() {
  std::vector<LightComponent*> lights;
  for (std::unique_ptr<GameObject>& obj : objects) {
    for (LightComponent* light : obj->getComponents<LightComponent>()) {
      if (light->isEnabled()) {
        lights.push_back(light);
      }
    }
  }
  LOG_DEBUG("Collected ", lights.size(), " lights");
  return lights;
}

void Scene::setLightUniforms(Shader* shader) {
  std::vector<LightComponent*> lights = collectLights();

  int numDir = 0, numPoint = 0, numSpot = 0;

  /* TODO: Collect max lights number from the shader */
  for (LightComponent* light : lights) {
    if (DirectionalLightComponent* dirLight =
            dynamic_cast<DirectionalLightComponent*>(light)) {
      if (numDir < 8) {
        dirLight->setUniforms(shader, numDir);
        numDir++;
      }
    } else if (PointLightComponent* pointLight =
                   dynamic_cast<PointLightComponent*>(light)) {
      if (numPoint < 8) {
        pointLight->setUniforms(shader, numPoint);
        numPoint++;
      }
    } else if (SpotlightComponent* spotLight =
                   dynamic_cast<SpotlightComponent*>(light)) {
      if (numSpot < 8) {
        spotLight->setUniforms(shader, numSpot);
        numSpot++;
      }
    }
  }

  LOG_DEBUG("Number of lights: ", numDir, " ", numPoint, " ", numSpot);
  shader->setUniform("numDirLights", numDir);
  shader->setUniform("numPointLights", numPoint);
  shader->setUniform("numSpotLights", numSpot);
}

bool needsLightning(Shader* shader) {
  return shader->hasUniform("numSpotLights");
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
