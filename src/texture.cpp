#include "src/texture.h"

#include <cassert>
#include <iostream>

#include "src/utils.h"

Texture::Texture(const std::filesystem::path& texturePath) {
  glGenTextures(1, &id);
  /* TODO: Maybe texture_array in the future */
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  unsigned char* data =
      stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
  if (!data) {
    std::cout << "ERROR::TEXTURE: Failed to load texture: " << texturePath
              << std::endl;
    throw;
  }

  /* This assumes there are only 3 or 4 channel textures! */
  assert(3 <= nrChannels);
  assert(4 >= nrChannels);
  unsigned int textureFormat = nrChannels == 3 ? GL_RGB : GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, textureFormat,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  checkGLError("glactivate");
  glBindTexture(GL_TEXTURE_2D, id);
  checkGLError("glbind");
}

Texture::~Texture() {
  glDeleteTextures(1, &id);
}
