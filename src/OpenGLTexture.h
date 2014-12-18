#ifndef _OPENGLTEXTURE_H_
#define _OPENGLTEXTURE_H_

#include "Texture.h"

#include <vector>
#include <cstddef>

namespace canvas {
  class TextureRef;

  class OpenGLTexture : public Texture {
  public:
    
  OpenGLTexture(unsigned int _width, unsigned int _height, FilterMode _min_filter, FilterMode _mag_filter)
    : Texture(_width, _height, _min_filter, _mag_filter) { }
    ~OpenGLTexture() {
      if (texture_id) {
	freed_textures.push_back(texture_id);
	total_textures--;
      }
    }

    unsigned int getTextureId() const { return texture_id; }
    
    void updateData(void * buffer);
    void updateData(void * buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

    static size_t getNumTextures() { return total_textures; }
    static const std::vector<unsigned int> & getFreedTextures() { return freed_textures; }
    static void releaseTextures();
    static TextureRef createTexture(unsigned int width, unsigned int height, FilterMode min_filter, FilterMode mag_filter);

  private:
    unsigned int texture_id = 0;
    unsigned int loaded_width = 0, loaded_height = 0;

    static size_t total_textures;
    static std::vector<unsigned int> freed_textures;
  };

  class OpenGLTextureFactory : public TextureFactory {
  public:
    OpenGLTextureFactory() { }
    
  };
};

#endif
