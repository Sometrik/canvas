#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "FilterMode.h"

namespace canvas {
  class Texture {
  public:
    friend class TextureRef;

  Texture(unsigned int _width, unsigned int _height, FilterMode _min_filter, FilterMode _mag_filter) 
    : width(_width), height(_height), min_filter(_min_filter), mag_filter(_mag_filter) { }
    virtual ~Texture() { }

    virtual void updateData(const void * buffer) = 0;
    virtual void updateData(const void * buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
    virtual unsigned int getTextureId() const { return 0; }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    FilterMode getMinFilter() const { return min_filter; }
    FilterMode getMagFilter() const { return mag_filter; }

  protected:
    void incRefcnt() { ++refcnt; }
    int decRefcnt();

  private:
    Texture(const Texture & other);
    Texture & operator=(const Texture & other);

    int refcnt = 0;
    unsigned int width, height;
    FilterMode min_filter;
    FilterMode mag_filter;
  };

  class TextureFactory {
  public:
    TextureFactory() { }
    
  };
};

#endif
