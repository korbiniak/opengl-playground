#include "src/text_mesh.h"

#include "src/logger.h"
#include "src/utils.h"

TextMesh::TextMesh(std::shared_ptr<FontAtlas> font, const std::string& text,
                   bool disableDepthMask)
    : Mesh(std::vector<Vertex>(), std::vector<unsigned int>()),
      font(std::move(font)),
      text(text),
      needsRebuild(false),
      disableDepthMask(disableDepthMask) {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  buildVertices();

  glBindVertexArray(VAO);
  Vertex::enableVertexAttribArray();

  glBindVertexArray(0);
}

void TextMesh::setText(const std::string& newTextMesh) {
  needsRebuild = true;
  text = newTextMesh;
}

void TextMesh::setFont(std::shared_ptr<FontAtlas> newFont) {
  needsRebuild = true;
  font = std::move(newFont);
}

void TextMesh::buildVertices() {
  vertices.clear();
  int x = 10;
  int y = 10;
  float z = 0;

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
        {{xpos, ypos + h, z}, {1.0F, 1.0F, 1.0F}, {texLeft, texTop}},
        {{xpos, ypos, z}, {1.0F, 1.0F, 1.0F}, {texLeft, texBottom}},
        {{xpos + w, ypos, z}, {1.0F, 1.0F, 1.0F}, {texRight, texBottom}},
        {{xpos, ypos + h, z}, {1.0F, 1.0F, 1.0F}, {texLeft, texTop}},
        {{xpos + w, ypos, z}, {1.0F, 1.0F, 1.0F}, {texRight, texBottom}},
        {{xpos + w, ypos + h, z}, {1.0F, 1.0F, 1.0F}, {texRight, texTop}},
    };

    vertices.insert(vertices.end(), glyphVertices.begin(), glyphVertices.end());

    x += (glyph.advance);
    /* So hacky... */
    z += 0.001;
  }

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_DYNAMIC_DRAW);
  glBindVertexArray(0);
}

void TextMesh::draw() {
  LOG_DEBUG("Drawing text mesh!");
  if (disableDepthMask) {
    glDepthMask(GL_FALSE);
  }
  /* TODO: This can be done much cleaner. This doesn't have to know anything
     about the shader, just draw the vertices */
  if (needsRebuild) {
    buildVertices();
    needsRebuild = false;
  }

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glBindVertexArray(0);

  if (disableDepthMask) {
    glDepthMask(GL_TRUE);
  }
}
