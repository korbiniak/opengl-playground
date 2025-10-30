#ifndef UITEXT_H
#define UITEXT_H

#include <memory>

#include "src/font_atlas.h"
#include "src/logger.h"
#include "src/shader.h"
#include "src/text_mesh.h"

class UIText {
 private:
  TextMesh textMesh;
  std::shared_ptr<FontAtlas> font;
  glm::vec2 screenPosition;
  glm::vec3 color;
  float scale;

 public:
  UIText(std::shared_ptr<FontAtlas> font, glm::vec2 pos, std::string text = "",
         glm::vec3 color = glm::vec3(1.0F), float scale = 1);

  void draw(Shader* shader, int screenWidth, int screenHeight);
};

#endif /* UITEXT_H */
