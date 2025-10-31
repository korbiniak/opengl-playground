#ifndef FONT_ATLAS_H
#define FONT_ATLAS_H

#include <optional>
#include <unordered_map>

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "src/texture.h"

class FontAtlas : public Texture {
 public:
  struct GlyphInfo {
    float x, y;
    float width, height;
    float bearingX, bearingY;
    FT_Pos advance;

    template <class Archive>
    void serialize(Archive& ar) {
      ar(x, y, width, height, bearingX, bearingY, advance);
    }
  };

 private:
  std::unordered_map<unsigned int, GlyphInfo> glyphs;
  float fontSize;
  float lineHeight;

  void setupTexture(unsigned char* data);

 public:
  /* Load atlas from file. */
  explicit FontAtlas(const std::filesystem::path& atlasPath);

  /* Generate atlas directly from font. */
  FontAtlas(const std::filesystem::path& fontPath, float fontSize);

  void saveAtlas(const std::filesystem::path& outputPath,
                 unsigned char* atlasData) const;

  float getFontSize() const { return fontSize; }
  float getLineHeight() const { return lineHeight; }

  GlyphInfo& getGlyph(unsigned int c);
};

#endif /* FONT_ATLAS_H */
