#ifndef TEXTURE_H
#define TEXTURE_H

#include <filesystem>

#include <GL/glew.h>
#include <stb_image.h>

class Texture {
 private:
  GLuint id;

 public:
  Texture(const std::filesystem::path& texturePath);

  void bind(unsigned int slot = 0) const;

  ~Texture();
};
#endif /* TEXTURE_H */
