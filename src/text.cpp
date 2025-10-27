#include "src/text.h"

#include "src/logger.h"
#include "src/utils.h"

Text::Text(std::shared_ptr<FontAtlas> font, const std::string& text,
           glm::vec3 color)
    : font(font), text(text), color(color), needsRebuild(false) {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  buildVertices();

  glBindVertexArray(VAO);
  GLint boundBuffer;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &boundBuffer);
  std::cout << "Bound VBO during attribute setup: " << boundBuffer << std::endl;
  Vertex::enableVertexAttribArray();

  glBindVertexArray(0);
}

Text::~Text() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

void Text::setText(const std::string& newText) {
  needsRebuild = true;
  text = newText;
}

void Text::setColor(glm::vec3 newColor) {
  needsRebuild = true;
  color = newColor;
}

void Text::setFont(std::shared_ptr<FontAtlas> newFont) {
  needsRebuild = true;
  font = std::move(newFont);
}

void Text::buildVertices() {
  vertices.clear();
  int x = 10;
  int y = 10;

  float atlasWidth = static_cast<float>(font->getWidth());
  float atlasHeight = static_cast<float>(font->getHeight());

  for (char c : text) {
    FontAtlas::GlyphInfo& glyph = font->getGlyph(c);
    float xpos = x + glyph.bearingX;
    float ypos = y - (glyph.height - glyph.bearingY);

    float w = glyph.width;
    float h = glyph.height;

    float texLeft = static_cast<float>(glyph.x) / atlasWidth;
    float texRight = static_cast<float>(glyph.x + glyph.width) / atlasWidth;
    float texTop = static_cast<float>(glyph.y) / atlasHeight;
    float texBottom = static_cast<float>(glyph.y + glyph.height) / atlasHeight;

    std::vector<Vertex> glyphVertices = {
        {{xpos, ypos + h, 0.0F}, {1.0F, 1.0F, 1.0F}, {texLeft, texTop}},
        {{xpos, ypos, 0.0F}, {1.0F, 1.0F, 1.0F}, {texLeft, texBottom}},
        {{xpos + w, ypos, 0.0F}, {1.0F, 1.0F, 1.0F}, {texRight, texBottom}},
        {{xpos, ypos + h, 0.0F}, {1.0F, 1.0F, 1.0F}, {texLeft, texTop}},
        {{xpos + w, ypos, 0.0F}, {1.0F, 1.0F, 1.0F}, {texRight, texBottom}},
        {{xpos + w, ypos + h, 0.0F}, {1.0F, 1.0F, 1.0F}, {texRight, texTop}},
    };

    vertices.insert(vertices.end(), glyphVertices.begin(), glyphVertices.end());

    x += (glyph.advance);
  }

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_DYNAMIC_DRAW);
  glBindVertexArray(0);
}

void Text::render(Shader& shader, const glm::mat4& transform) {
  glDepthMask(GL_FALSE);
  shader.use();
  shader.setUniform("textColor", color);
  shader.setUniform("transform", transform);

  if (needsRebuild) {
    buildVertices();
    needsRebuild = false;
  }

  font->bind(0);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glBindVertexArray(0);
  glDepthMask(GL_TRUE);
}
