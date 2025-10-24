#include "src/resource_manager.h"

#include "src/exceptions.h"
#include "src/logger.h"

template <typename T, typename... Args>
std::shared_ptr<T> ResourceManager::loadResource(
    const std::string& resourceType,
    std::unordered_map<std::string, std::shared_ptr<T>>& container,
    const std::string& name, Args&&... args) {

  LOG_INFO("Loading ", resourceType, ": ", name);

  if (container.count(name)) {
    std::string message = resourceType + " '" + name + "' already exists";
    LOG_ERROR(message);
    throw ResourceException("Failed to load " + resourceType + " '" + name +
                            "': " + message);
  }

  try {
    std::shared_ptr<T> resource =
        std::make_shared<T>(std::forward<Args>(args)...);
    container[name] = resource;
    LOG_INFO(resourceType, " '", name, "' successfully loaded");
    return resource;
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to load ", resourceType, " '", name, "': ", e.what());
    throw;
  }
}

template <typename T>
std::shared_ptr<T> ResourceManager::getResource(
    const std::string& resourceType,
    const std::unordered_map<std::string, std::shared_ptr<T>>& container,
    const std::string& name) {

  if (!container.count(name)) {
    LOG_WARNING(resourceType, " '", name, "' does not exist");
    return nullptr;
  }
  return container.at(name);
}

std::shared_ptr<Shader> ResourceManager::loadShader(
    const std::string& name, const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath) {
  return loadResource<Shader>("shader", shaders, name, vertexPath,
                              fragmentPath);
}

std::shared_ptr<Mesh> ResourceManager::loadMesh(
    const std::string& name, const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices) {
  return loadResource<Mesh>("mesh", meshes, name, vertices, indices);
}

std::shared_ptr<Texture2D> ResourceManager::loadTexture(
    const std::string& name, const std::filesystem::path& path) {
  return loadResource<Texture2D>("texture", textures, name, path);
}

std::shared_ptr<FontAtlas> ResourceManager::loadFont(
    const std::string& name, const std::filesystem::path& path,
    float fontSize) {
  return loadResource<FontAtlas>("font", fonts, name, path, fontSize);
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string& name) {
  return getResource<Shader>("Shader", shaders, name);
}

std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string& name) {
  return getResource<Mesh>("Mesh", meshes, name);
}

std::shared_ptr<Texture2D> ResourceManager::getTexture(
    const std::string& name) {
  return getResource<Texture2D>("Texture", textures, name);
}

std::shared_ptr<FontAtlas> ResourceManager::getFont(const std::string& name) {
  return getResource<FontAtlas>("Font", fonts, name);
}
