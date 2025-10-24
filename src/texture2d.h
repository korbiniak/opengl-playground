#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <GL/glew.h>

#include "src/texture.h"

class Texture2D : public Texture {
 public:
  explicit Texture2D(const std::filesystem::path& texturePath);

  Texture2D(const std::filesystem::path& texturePath, GLint wrapS, GLint wrapT,
            GLint minFilter, GLint magFilter);
};

#endif /* TEXTURE2D_H */
