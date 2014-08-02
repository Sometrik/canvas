#ifndef _SURFACE_H_
#define _SURFACE_H_

#include "../../personal/graphviewer/TextureLink.h"
#include "Color.h"

namespace canvas {
  class Surface {
  public:
    enum FilterMode {
      NEAREST = 1,
      LINEAR,
      LINEAR_MIPMAP_LINEAR
    };

    Surface(unsigned int _width, unsigned int _height)
      : texture(_width, _height),
      width(_width),
      height(_height) { }
    virtual ~Surface() { }

    virtual void resize(unsigned int _width, unsigned int _height) {
      texture.setWidth(_width);
      texture.setHeight(_height);
      width = _width;
      height = _height;
    }

    virtual void flush() { }
    virtual unsigned char * getBuffer() = 0;
    virtual const unsigned char * getBuffer() const = 0;

    void gaussianBlur(float hradius, float vradius);
    void colorize(const Color & color);

    const TextureLink & updateTexture();

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }

    void setMagFilter(FilterMode mode) { mag_filter = mode; }
    void setMinFilter(FilterMode mode) { min_filter = mode; }

    TextureLink texture;

  protected:

  private:
    Surface(const Surface & other) { }
    Surface & operator=(const Surface & other) {
      return *this;
    }

    unsigned int width, height;
    FilterMode mag_filter = LINEAR;
    FilterMode min_filter = LINEAR;
  };
};

#endif
