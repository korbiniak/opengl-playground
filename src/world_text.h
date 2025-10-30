#ifndef WORLD_TEXT_H
#define WORLD_TEXT_H

#include "src/font_atlas.h"
#include "src/game_object.h"
#include "src/text_mesh.h"

class WorldText : public GameObject {
 public:
  WorldText(std::shared_ptr<FontAtlas> font, std::shared_ptr<Material> material,
            const std::string& text)
      : GameObject(std::make_shared<TextMesh>(std::move(font), text),
                   std::move(material)) {}

  void setText(const std::string& text) {
    dynamic_cast<TextMesh*>(mesh.get())->setText(text);
  }
};

#endif /* WORLD_TEXT_H */
