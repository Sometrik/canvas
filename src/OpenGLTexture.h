#ifndef _OPENGLTEXTURE_H_
#define _OPENGLTEXTURE_H_

#include "Texture.h"

#include <vector>
#include <cstddef>

namespace canvas {
  class TextureRef;

  class OpenGLTexture : public Texture {
  public:
    
  OpenGLTexture(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, FilterMode _min_filter, FilterMode _mag_filter, InternalFormat _internal_format, unsigned int _mipmap_levels = 8)
    : Texture(_logical_width, _logical_height, _actual_width, _actual_height, _min_filter, _mag_filter, _internal_format, _mipmap_levels) { }
    ~OpenGLTexture() {
      if (texture_id) {
	freed_textures.push_back(texture_id);
	total_textures--;
      }
    }

    unsigned int getTextureId() const { return texture_id; }
    
    void updateData(const Image & image, unsigned int x, unsigned int y);

    static size_t getNumTextures() { return total_textures; }
    static const std::vector<unsigned int> & getFreedTextures() { return freed_textures; }
    static void releaseTextures();
    static TextureRef createTexture(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, FilterMode min_filter, FilterMode mag_filter, InternalFormat _internal_format, unsigned int mipmap_levels = 8);

  protected:
    void updateCompressedData(const Image & image, unsigned int x, unsigned int y);
    void updatePlainData(const Image & image, unsigned int x, unsigned int y);

  private:
    unsigned int texture_id = 0;

    static size_t total_textures;
    static std::vector<unsigned int> freed_textures;
  };

  class OpenGLTextureFactory : public TextureFactory {
  public:
    OpenGLTextureFactory() { }
    
  };
};

#endif
