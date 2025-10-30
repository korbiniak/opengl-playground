#include "src/uitext.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

UIText::UIText(std::shared_ptr<FontAtlas> font, glm::vec2 pos, std::string text,
               glm::vec3 color, float scale)
    : textMesh(font, text, true),
      font(std::move(font)),
      screenPosition(pos),
      color(color),
      scale(scale) {}

void UIText::draw(Shader* shader, int screenWidth, int screenHeight) {
  glm::mat4 projection =
      glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), glm::vec3(screenPosition, 0.0f));
  model = glm::scale(model, glm::vec3(scale));

  LOG_DEBUG("Drawing text: ", textMesh.getText());

  font->bind(0);
  shader->setUniform("fontAtlas", 0);
  shader->setUniform("model", projection * model);
  shader->setUniform("textColor", color);
  textMesh.draw();
}
