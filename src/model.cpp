#include "src/model.h"

#include "src/logger.h"
#include "src/utils.h"

Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
    : mesh(mesh), texture(texture) {}

void Model::draw(Shader& shader) {
  // LOG_DEBUG("Drawing!");
  shader.use();
  shader.setUniform("model", modelMatrix);
  if (texture) {
    texture->bind(0);
    shader.setUniform("texture1", 0);
  }
  mesh->draw();
}
