#include "src/texture.h"

#include <cassert>
#include <iostream>

#include <stb_image.h>

#include "src/utils.h"

Texture::Texture() : id(0), width(0), height(0), channels(0) {
  glGenTextures(1, &id);
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  checkGLError("glactivate");
  glBindTexture(GL_TEXTURE_2D, id);
  checkGLError("glbind");
}

void Texture::unbind() const {
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
  glDeleteTextures(1, &id);
}
