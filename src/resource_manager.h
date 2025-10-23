#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "src/mesh.h"
#include "src/shader.h"
#include "src/texture.h"

class ResourceManager {
 private:
  std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
  std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
  std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

 public:
  std::shared_ptr<Shader> loadShader(const std::string& name,
                                     const std::filesystem::path& vertexPath,
                                     const std::filesystem::path& fragmentPath);

  std::shared_ptr<Mesh> loadMesh(const std::string& name,
                                 const std::vector<Vertex>& vertices,
                                 const std::vector<unsigned int>& indices);

  std::shared_ptr<Texture> loadTexture(
      const std::string& name, const std::filesystem::path& fragmentPath);

  std::shared_ptr<Shader> getShader(const std::string& name);

  std::shared_ptr<Mesh> getMesh(const std::string& name);

  std::shared_ptr<Texture> getTexture(const std::string& name);
};

#endif /* RESOURCE_MANAGER_H */
