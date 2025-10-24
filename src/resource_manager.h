#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "src/font_atlas.h"
#include "src/mesh.h"
#include "src/shader.h"
#include "src/texture2d.h"

class ResourceManager {
 private:
  std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
  std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
  std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
  std::unordered_map<std::string, std::shared_ptr<FontAtlas>> fonts;

  template <typename T, typename... Args>
  std::shared_ptr<T> loadResource(
      const std::string& resourceType,
      std::unordered_map<std::string, std::shared_ptr<T>>& container,
      const std::string& name, Args&&... args);

  template <typename T>
  std::shared_ptr<T> getResource(
      const std::string& resourceType,
      const std::unordered_map<std::string, std::shared_ptr<T>>& container,
      const std::string& name);

 public:
  std::shared_ptr<Shader> loadShader(const std::string& name,
                                     const std::filesystem::path& vertexPath,
                                     const std::filesystem::path& fragmentPath);

  std::shared_ptr<Mesh> loadMesh(const std::string& name,
                                 const std::vector<Vertex>& vertices,
                                 const std::vector<unsigned int>& indices);

  std::shared_ptr<Texture2D> loadTexture(
      const std::string& name, const std::filesystem::path& fragmentPath);

  std::shared_ptr<FontAtlas> loadFont(const std::string& name,
                                      const std::filesystem::path& path,
                                      float fontSize);

  std::shared_ptr<Shader> getShader(const std::string& name);

  std::shared_ptr<Mesh> getMesh(const std::string& name);

  std::shared_ptr<Texture2D> getTexture(const std::string& name);

  std::shared_ptr<FontAtlas> getFont(const std::string& name);
};

#endif /* RESOURCE_MANAGER_H */
