#include "src/texture2d.h"

#include <stdexcept>

#include "src/logger.h"
#include "src/utils.h"

Texture2D::Texture2D(const std::filesystem::path& texturePath)
    : Texture2D(texturePath, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
                GL_LINEAR) {}

Texture2D::Texture2D(const std::filesystem::path& texturePath, GLint wrapS,
                     GLint wrapT, GLint minFilter, GLint magFilter)
    : Texture() {
  LOG_INFO("Loading texture from ", texturePath);
  stbi_set_flip_vertically_on_load(true);

  unsigned char* data =
      stbi_load(texturePath.c_str(), &width, &height, &channels, 0);

  if (!data) {
    LOG_ERROR("Failed to load texture: ", texturePath);
    throw std::runtime_error("Failed to load texture: " + texturePath.string());
  }

  GLenum format = GL_RGB;
  if (channels == 1) {
    format = GL_RED;
  } else if (channels == 3) {
    format = GL_RGB;
  } else if (channels == 4) {
    format = GL_RGBA;
  } else {
    LOG_WARNING("Texture with weird channel count: ", channels);
  }

  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
}
