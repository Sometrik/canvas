#ifndef _SURFACE_H_
#define _SURFACE_H_

#include "Color.h"

namespace canvas {
  class Surface {
  public:
    Surface() { }
    virtual ~Surface() { }

    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;
    virtual unsigned char * getBuffer() = 0;
    virtual const unsigned char * getBuffer() const = 0;

    void gaussianBlur(float hradius, float vradius);
    void colorize(const Color & color);

  private:
    Surface(const Surface & other) { }
    Surface & operator=(const Surface & other) {
      return *this;
    }
  };
};

#endif
