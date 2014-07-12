#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>

namespace canvas {
  class Context {
  public:
    Context() { }
    virtual ~Context() { }
    
    void clearRect(float x, float y, float w, float h) = 0;
    void fillRect(float x, float y, float w, float h) = 0;
    void moveTo(float x, float y) = 0;
    void lineTo(float x, float y) = 0;
    void stroke() = 0;
    void fill() = 0;

    float lineWidth;
    std::string strokeStyle;
    std::string fillStyle;
    std::string strokeStyle;

  private:
  };
};

#endif
