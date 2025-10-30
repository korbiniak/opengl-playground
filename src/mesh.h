#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <GL/glew.h>

#include "src/vertex.h"

class Mesh {
 private:
  std::string name;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  size_t indicesCount;

 public:
  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<unsigned int>& indices);

  void draw();

  const std::string& getName() const { return name; }
  void setName(const std::string& n) { name = n; }

  ~Mesh();
};

#endif /* MESH_H */
