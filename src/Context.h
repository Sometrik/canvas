#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>

#include "Style.h"

namespace canvas {  
  class Context {
  public:
    Context() { }
    virtual ~Context() { }
    
    virtual void clearRect(float x, float y, float w, float h) = 0;
    virtual void fillRect(float x, float y, float w, float h) = 0;
    virtual void moveTo(float x, float y) = 0;
    virtual void lineTo(float x, float y) = 0;
    virtual void stroke() = 0;
    virtual void fill() = 0;

    float lineWidth;
    Style fillStyle;
    Style strokeStyle;

  private:
  };
};

#endif
