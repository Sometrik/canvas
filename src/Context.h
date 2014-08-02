#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>
#include <memory>

#include "Color.h"
#include "Style.h"
#include "Font.h"
#include "Surface.h"

namespace canvas {  
  struct Size {
    float width, height;
  };

  class Context {
  public:
    Context(unsigned int _width, unsigned int _height)
      : width(_width), height(_height) { }
    virtual ~Context() { }

    virtual std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, unsigned char * data) = 0;

    virtual void check() const { }

    virtual void resize(unsigned int _width, unsigned int _height);
    
    virtual void beginPath() = 0;
    virtual void closePath() = 0;
    virtual void clip() = 0;
    virtual void save() = 0;
    virtual void restore() = 0;

    virtual void arc(double x, double y, double r, double a0, double a1, bool t = false) = 0;    
    virtual void clearRect(double x, double y, double w, double h) = 0;

    virtual void moveTo(double x, double y) = 0;
    virtual void lineTo(double x, double y) = 0;
    virtual void stroke() = 0;
    virtual void fill() = 0;
    virtual void fillText(const std::string & text, double x, double y) = 0;
    virtual Size measureText(const std::string & text) = 0;
    
    void fillRect(double x, double y, double w, double h);
    void strokeRect(double x, double y, double w, double h);
    
    virtual Surface & getDefaultSurface() = 0;
    virtual const Surface & getDefaultSurface() const = 0;

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }

    virtual void drawImage(Context & other, double x, double y, double w, double h) {
      drawImage(other.getDefaultSurface(), x, y, w, h);
    }
    virtual void drawImage(Surface & img, double x, double y, double w, double h) = 0;

    float lineWidth = 1.0f;
    Style fillStyle;
    Style strokeStyle;
    float shadowBlur = 0.0f;
    Color shadowColor;
    float shadowOffsetX = 0.0f, shadowOffsetY = 0.0f;
    float globalAlpha = 1.0f;
    Font font;
    
  protected:
#if 0
    void setDefaultSurface(std::shared_ptr<Surface> & s) { default_surface = s; }
#endif

  private:
    unsigned int width, height;
#if 0
    std::shared_ptr<Surface> default_surface;
#endif

    Context(const Context & other) { }
    Context & operator=(const Context & other) { return *this; }
  };
};

#endif
