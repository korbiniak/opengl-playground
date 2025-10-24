#ifndef TEXT_H
#define TEXT_H

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "src/font_atlas.h"
#include "src/shader.h"
#include "src/vertex.h"

class Text {
 private:
  std::shared_ptr<FontAtlas> font;
  std::string text;
  glm::vec3 color;

  GLuint VAO, VBO;

  std::vector<Vertex> vertices;
  bool needsRebuild;

  void buildVertices();

 public:
  Text(std::shared_ptr<FontAtlas> font, const std::string& text = "",
       glm::vec3 color = glm::vec3(1.0f));

  ~Text();

  void setText(const std::string& newText);
  void setColor(glm::vec3 newColor);
  void setFont(std::shared_ptr<FontAtlas> newFont);

  const std::string& getText() const { return text; }
  const glm::vec3& getColor() const { return color; }

  void render(Shader& shader, const glm::mat4& transform);
  GLuint getVAO() { return VAO; }
};

#endif /* TEXT_H */
