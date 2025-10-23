#ifndef MODEL_H
#define MODEL_H

#include "src/game_object.h"
#include "src/mesh.h"
#include "src/texture.h"

#include <memory>

class Model : public GameObject {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Texture> texture;

 public:
  Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);

  void draw(Shader& shader) override;
};

#endif /* MODEL_H */
