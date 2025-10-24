#ifndef TEXTURE_H
#define TEXTURE_H

#include <filesystem>

#include <GL/glew.h>
#include <stb_image.h>

class Texture {
 protected:
  GLuint id;
  int width;
  int height;
  int channels;

  Texture();

 public:
  virtual ~Texture();

  void bind(unsigned int slot = 0) const;
  void unbind() const;

  GLuint getId() const { return id; }
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int getChannels() const { return channels; }
};
#endif /* TEXTURE_H */
