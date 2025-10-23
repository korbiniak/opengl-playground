#include "src/resource_manager.h"

#include "src/exceptions.h"
#include "src/logger.h"

std::shared_ptr<Shader> ResourceManager::loadShader(
    const std::string& name, const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath) {
  LOG_INFO("Loading shader: ", name);

  if (shaders.count(name)) {
    std::string message = "shader '" + name + "' already exists";
    LOG_ERROR(message);
    throw ResourceException("Failed to load shader '" + name + "': " + message);
  }

  try {
    std::shared_ptr<Shader> shader =
        std::make_shared<Shader>(vertexPath, fragmentPath);
    shaders[name] = shader;
    LOG_INFO("Shader '" + name + "' successfuly loaded");
    return shader;
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to load shader '" + name + "': ", e.what());
    throw;
  }
}

std::shared_ptr<Mesh> ResourceManager::loadMesh(
    const std::string& name, const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices) {

  LOG_INFO("Loading mesh: ", name);

  if (meshes.count(name)) {
    std::string message = "mesh '" + name + "' already exists";
    LOG_ERROR(message);
    throw ResourceException("Failed to load mesh '" + name + "': " + message);
  }

  try {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertices, indices);
    meshes[name] = mesh;
    LOG_INFO("Mesh '" + name + "' successfuly loaded");
    return mesh;
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to load mesh '" + name + "': ", e.what());
    throw;
  }
}

std::shared_ptr<Texture> ResourceManager::loadTexture(
    const std::string& name, const std::filesystem::path& path) {

  LOG_INFO("Loading texture: ", name);

  if (textures.count(name)) {
    std::string message = "texture '" + name + "' already exists";
    LOG_ERROR(message);
    throw ResourceException("Failed to load texture '" + name +
                            "': " + message);
  }

  try {
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(path);
    textures[name] = texture;
    LOG_INFO("Texture '" + name + "' successfuly loaded");
    return texture;
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to load texture '" + name + "': ", e.what());
    throw;
  }
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string& name) {
  if (!shaders.count(name)) {
    LOG_WARNING("Shader '", name, "' does not exist");
    return nullptr;
  }

  return shaders[name];
}

std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string& name) {
  if (!meshes.count(name)) {
    LOG_WARNING("Mesh '", name, "' does not exist");
    return nullptr;
  }

  return meshes[name];
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& name) {
  if (!textures.count(name)) {
    LOG_WARNING("Texture '", name, "' does not exist");
    return nullptr;
  }

  return textures[name];
}
