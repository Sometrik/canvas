#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "FilterMode.h"

namespace canvas {
  class Texture {
  public:
    friend class TextureLink;

    Texture(unsigned int _width, unsigned int _height) 
      : width(_width), height(_height) { }
    virtual ~Texture() { }

    virtual void updateData(unsigned char * buffer) = 0;
    virtual unsigned int getTextureId() const { return 0; }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    FilterMode getMagFilter() const { return mag_filter; }
    FilterMode getMinFilter() const { return min_filter; }

    void incRefcnt() { ++refcnt; }
    unsigned int decRefcnt() { return --refcnt; }

  private:
    Texture(const Texture & other);
    Texture & operator=(const Texture & other);

    unsigned int refcnt = 0;
    unsigned int width, height;
    FilterMode mag_filter = LINEAR;
    FilterMode min_filter = LINEAR;
  };

  class TextureFactory {
  public:
    TextureFactory() { }
    
  };
};

#endif
