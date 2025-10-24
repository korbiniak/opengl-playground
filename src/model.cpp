#include "src/model.h"

#include "src/logger.h"
#include "src/utils.h"

Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
    : mesh(mesh), texture(texture) {}

void Model::draw(Shader& shader) {
  shader.use();
  shader.setUniform("model", modelMatrix);
  checkGLError("after setuniform");
  if (texture) {
    texture->bind(0);
    checkGLError("after bind");
    shader.setUniform("texture1", 0);
    checkGLError("after setunifrom texture");
  }
  mesh->draw();
  checkGLError("after mesh->draw");
}
