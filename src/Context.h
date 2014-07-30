#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>

#include "Color.h"
#include "Style.h"
#include "Font.h"

namespace canvas {  
  struct Size {
    float width, height;
  };

  class Context {
  public:
    Context() { }
    virtual ~Context() { }

    virtual void beginPath() = 0;
    virtual void closePath() = 0;
    virtual void clip() = 0;

    virtual void arc(double x, double y, double r, double a0, double a1, bool t) = 0;    
    virtual void clearRect(double x, double y, double w, double h) = 0;
    virtual void fillRect(double x, double y, double w, double h) = 0;
    virtual void moveTo(double x, double y) = 0;
    virtual void lineTo(double x, double y) = 0;
    virtual void stroke() = 0;
    virtual void fill() = 0;
    virtual void fillText(const std::string & text, double x, double y) = 0;
    virtual Size measureText(const std::string & text) = 0;

    virtual void drawImage(Context & other, double x, double y, double w, double h) = 0;

    float lineWidth = 1.0f;
    Style fillStyle;
    Style strokeStyle;
    float shadowBlur = 0.0f;
    Color shadowColor;
    float shadowOffsetX = 0.0f, shadowOffsetY = 0.0f;
    float globalAlpha = 1.0f;
    Font font;
    
  private:
    Context(const Context & other) { }
    Context & operator=(const Context & other) { return *this; }
  };
};

#endif
