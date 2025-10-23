#ifndef MESH_H
#define MESH_H

#include <vector>

#include <GL/glew.h>

#include "src/vertex.h"

class Mesh {
 private:
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  size_t indicesCount;

 public:
  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<unsigned int>& indices);

  void draw();

  ~Mesh();
};

#endif /* MESH_H */
