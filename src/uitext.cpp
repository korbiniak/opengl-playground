#include "src/uitext.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

UIText::UIText(std::shared_ptr<FontAtlas> font, glm::vec2 pos, std::string text,
               glm::vec3 color, float scale)
    : textRenderer(std::move(font), text, color),
      screenPosition(pos),
      scale(scale) {}

void UIText::render(Shader& shader, int screenWidth, int screenHeight) {
  glm::mat4 projection =
      glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), glm::vec3(screenPosition, 0.0f));
  model = glm::scale(model, glm::vec3(scale));
  glm::mat4 transform = projection * model;

  textRenderer.render(shader, transform);
}
