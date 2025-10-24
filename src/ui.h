#ifndef UI_H
#define UI_H

#include <memory>

#include "src/font_atlas.h"
#include "src/uitext.h"

class UI {
 private:
  /* TODO: map from names to texts */
  std::unordered_map<std::string, std::unique_ptr<UIText>> texts;

 public:
  void addText(std::string name, std::shared_ptr<FontAtlas> font,
               glm::vec2 position, const std::string& text, float scale) {
    /* TODO wiadomo */
    texts.insert(std::make_pair(
        name, std::make_unique<UIText>(std::move(font), position, text,
                                       glm::vec3(1.0F), scale)));
  }

  void render(Shader& shader, int screenWidth, int screenHeight) {
    shader.use();
    for (auto& text : texts) {
      text.second->render(shader, screenWidth, screenHeight);
    }
  }
};

#endif
