#ifndef TEXT_H
#define TEXT_H

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "src/font_atlas.h"
#include "src/mesh.h"
#include "src/shader.h"
#include "src/vertex.h"

class TextMesh : public Mesh {
 private:
  std::shared_ptr<FontAtlas> font;
  std::string text;

  bool needsRebuild;
  bool disableDepthMask;

  void buildVertices();

 public:
  TextMesh(std::shared_ptr<FontAtlas> font, const std::string& text = "",
           bool disableDepthMask = false);

  void setText(const std::string& newText);
  void setFont(std::shared_ptr<FontAtlas> newFont);

  const std::string& getText() const { return text; }

  void draw() override;
};

#endif /* TEXT_H */
