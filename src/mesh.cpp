#include "src/mesh.h"

#include "src/logger.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices) {
  indicesCount = indices.size();

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  Vertex::enableVertexAttribArray();
  glBindVertexArray(0);
}

void Mesh::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
