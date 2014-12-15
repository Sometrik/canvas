#ifndef _SURFACE_H_
#define _SURFACE_H_

#include "TextureRef.h"
#include "Color.h"
#include "FilterMode.h"
#include "Path.h"
#include "Style.h"
#include "Font.h"
#include "TextBaseline.h"
#include "TextAlign.h"

#include <memory>

namespace canvas {
  class Context;
  class Image;

  class Surface {
  public:
    friend class Context;

    Surface(unsigned int _width, unsigned int _height)
      : texture(_width, _height),
      width(_width),
      height(_height) { }

    virtual ~Surface() {
      delete[] scaled_buffer;
    }

    virtual void resize(unsigned int _width, unsigned int _height) {
      texture.setWidth(_width);
      texture.setHeight(_height);
      width = _width;
      height = _height;
    }

    virtual void flush() { }
    virtual void markDirty() { }

    virtual Surface * copy() = 0;
    virtual void * lockMemory(bool write_access = false, unsigned int scaled_width = 0, unsigned int scaled_height = 0) = 0;
    virtual void * lockMemoryPartial(unsigned int x0, unsigned int y0, unsigned int required_width, unsigned int required_height);
    virtual void releaseMemory() {
      delete[] scaled_buffer;
      scaled_buffer = 0;
    }

    virtual void clip(const Path & path) = 0;
    virtual void stroke(const Path & path, const Style & style, double lineWidth) = 0;
    virtual void fill(const Path & path, const Style & style) = 0;
    
    void colorFill(const Color & color);
    void gaussianBlur(float hradius, float vradius);
    void multiply(const Color & color);
    
    const TextureRef & getTexture() const { return texture; }
    const TextureRef & updateTexture();
    const TextureRef & updateTexture(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

    std::shared_ptr<Image> createImage(unsigned int required_width = 0, unsigned int required_height = 0);

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }

    void setMagFilter(FilterMode mode) { mag_filter = mode; }
    void setMinFilter(FilterMode mode) { min_filter = mode; }
  
    virtual void fillText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) = 0;
    virtual void strokeText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) = 0;
    virtual void drawImage(Surface & _img, double x, double y, double w, double h, float alpha = 1.0f) = 0;
    virtual void save() = 0;
    virtual void restore() = 0;
    
  protected:
    TextureRef texture;

  private:
    Surface(const Surface & other) { }
    Surface & operator=(const Surface & other) {
      return *this;
    }

    unsigned int width, height;
    FilterMode mag_filter = LINEAR;
    FilterMode min_filter = LINEAR;
    unsigned int * scaled_buffer = 0;
  };

  class NullSurface : public Surface {
  public:
    void fillText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) { }
    void strokeText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) { }
    void drawImage(Surface & _img, double x, double y, double w, double h) { }
  };
};

#endif
