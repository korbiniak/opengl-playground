#include "src/scene.h"

#include <memory>

#include "src/directional_light_component.h"
#include "src/game_object.h"
#include "src/light_component.h"
#include "src/point_light_component.h"
#include "src/spotlight_component.h"

void Scene::update(float deltaTime) {
  forEachObject([deltaTime](GameObject* obj) { obj->update(deltaTime); });
}

int priority(bool opaque) {
  /* 0 if opaque, 1 otherwise */
  return !opaque;
}

/* Probably better if grouped by shader and not material */
std::map<std::pair<int, Material*>, std::vector<GameObject*>>
Scene::groupByMaterial() {
  std::map<std::pair<int, Material*>, std::vector<GameObject*>> groups;

  forEachObject([&groups](GameObject* obj) {
    Material* mat = obj->getMaterial().get();
    if (mat) {
      groups[std::make_pair(priority(mat->isOpaque()), mat)].push_back(obj);
    }
  });

  return groups;
}

std::vector<LightComponent*> Scene::collectLights() {
  std::vector<LightComponent*> lights;

  forEachObject([&lights](GameObject* obj) {
    for (LightComponent* light : obj->getComponents<LightComponent>()) {
      if (light->isEnabled()) {
        lights.push_back(light);
      }
    }
  });

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
  Camera* camera = getActiveCamera();

  const glm::mat4& viewMatrix = camera->getViewMatrix();
  const glm::mat4& projectionMatrix = camera->getProjectionMatrix();
  const glm::vec3 cameraPosition = camera->getPosition();

  std::map<std::pair<int, Material*>, std::vector<GameObject*>> groups =
      groupByMaterial();

  for (auto [key, group] : groups) {
    auto material = key.second;

    Shader* shader = material->getShader().get();
    material->bind();
    shader->setUniform("view", viewMatrix);
    shader->setUniform("projection", projectionMatrix);

    if (needsLightning(shader)) {
      setLightUniforms(shader);
    }
    if (needsCameraPosition(shader)) {
      shader->setUniform("viewPos", cameraPosition);
    }
    for (GameObject* obj : group) {
      obj->drawGeometry();
    }
  }
}

uint64_t Scene::findMaxGameObjectId() const {
  uint64_t maxId = 0;

  for (const auto& root : rootObjects) {
    maxId = std::max(maxId, root->getId());
    root->forEachChild([&maxId](GameObject* child) {
      maxId = std::max(maxId, child->getId());
    });
  }

  return maxId;
}

void Scene::updateNextGameObjectId() {
  uint64_t maxId = findMaxGameObjectId();
  GameObject::setNextId(maxId + 1);
}
