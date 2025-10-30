#include "src/font_atlas.h"

#include <bit>
#include <fstream>
#include <stdexcept>

#include <stb_image_write.h>
#include <cereal/archives/binary.hpp>
#include <cereal/types/unordered_map.hpp>

#include "freetype/freetype.h"
#include "src/logger.h"

void FontAtlas::setupTexture(unsigned char* data) {
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);
}

FontAtlas::FontAtlas(const std::filesystem::path& atlasPath) : Texture() {
  LOG_INFO("Loading font atlas from ", atlasPath);
  stbi_set_flip_vertically_on_load(false);

  unsigned char* data =
      stbi_load(atlasPath.c_str(), &width, &height, &channels, 0);

  if (!data) {
    LOG_ERROR("Failed to load font atlas: ", atlasPath);
    throw std::runtime_error("Failed to load font atlas: " +
                             atlasPath.string());
  }

  setupTexture(data);
  stbi_image_free(data);

  std::filesystem::path metaPath = atlasPath.string() + ".meta";
  std::ifstream metaFile(metaPath, std::ios::binary);

  if (!metaFile) {
    LOG_ERROR("Failed to open font atlas metadata: ", metaPath);
    throw std::runtime_error("Failed to open font atlas metadata: " +
                             metaPath.string());
  }

  cereal::BinaryInputArchive archive(metaFile);
  archive(fontSize, lineHeight, glyphs);
}

FontAtlas::FontAtlas(const std::filesystem::path& fontPath, float fontSize)
    : Texture(), fontSize(fontSize) {
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    LOG_ERROR("FreeType library init failed.");
    throw std::runtime_error("FreeType library init failed.");
  }

  FT_Face face;
  if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
    LOG_ERROR("Failed to load font from ", fontPath);
    throw std::runtime_error("Failed to load font from " + fontPath.string());
  }

  FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(fontSize));

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  const int atlasWidth = 1024;
  const int atlasHeight = 1024;

  width = atlasWidth;
  height = atlasHeight;
  channels = 1;

  std::vector<unsigned char> atlasData(atlasWidth * atlasHeight, 0);
  int x = 0, y = 0;
  int rowHeight = 0;
  lineHeight = face->size->metrics.height / 64.0f;

  for (unsigned char c = 32; c < 127; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      LOG_WARNING("Failed to load glyph: ", c);
      continue;
    }

    FT_GlyphSlot g = face->glyph;
    FT_Render_Glyph(g, FT_RENDER_MODE_SDF);

    if (x + g->bitmap.width >= atlasWidth) {
      x = 0;
      y += rowHeight;
      rowHeight = 0;
    }

    if (y + g->bitmap.rows >= atlasHeight) {
      LOG_WARNING("Atlas too small to load all glyphs! Last loaded: ", (int)c);
      break;
    }

    for (unsigned int row = 0; row < g->bitmap.rows; ++row) {
      for (unsigned int col = 0; col < g->bitmap.width; ++col) {
        int atlasX = x + col;
        int atlasY = y + row;
        atlasData[atlasY * atlasWidth + atlasX] =
            g->bitmap.buffer[row * g->bitmap.width + col];
      }
    }

    GlyphInfo info;
    info.x = static_cast<float>(x);
    info.y = static_cast<float>(y);
    info.width = static_cast<float>(g->bitmap.width);
    info.height = static_cast<float>(g->bitmap.rows);
    info.bearingX = static_cast<float>(g->bitmap_left);
    info.bearingY = static_cast<float>(g->bitmap_top);
    info.advance = static_cast<float>(g->advance.x / 64.0f);

    glyphs[c] = info;

    x += g->bitmap.width + 1;
    rowHeight = std::max(rowHeight, static_cast<int>(g->bitmap.rows));
  }

  setupTexture(atlasData.data());
  saveAtlas("atlas.jpg", atlasData.data());

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void FontAtlas::saveAtlas(const std::filesystem::path& outputPath,
                          unsigned char* atlasData) const {
  stbi_write_jpg(outputPath.c_str(), width, height, 1, (void*)atlasData, 100);

  std::filesystem::path metaPath = outputPath.string() + ".meta";
  std::ofstream metaFile(metaPath, std::ios::binary);

  if (!metaFile) {
    LOG_ERROR("Failed to save font atlas metadata to ", outputPath);
    throw std::runtime_error("Failed to save font atlas metadata to " +
                             outputPath.string());
  }

  cereal::BinaryOutputArchive archive(metaFile);
  archive(fontSize, lineHeight, glyphs);
}

FontAtlas::GlyphInfo& FontAtlas::getGlyph(char c) {
  auto it = glyphs.find(c);
  if (it != glyphs.end()) {
    return it->second;
  }

  LOG_WARNING("Unkown character in the atlas: ", c, "(", (unsigned int)c, ")");
  assert(glyphs.count('?'));
  return glyphs.find('?')->second;
}
