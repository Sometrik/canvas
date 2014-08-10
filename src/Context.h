#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>
#include <memory>

#include "Color.h"
#include "Style.h"
#include "Font.h"
#include "Surface.h"
#include "Image.h"
#include "TextBaseline.h"

namespace canvas {  
  class Point {
  public:
  Point(double _x, double _y) : x(_x), y(_y) { }
    double x, y;
  };
  struct TextMetrics {
    float width, height;
  };
  enum TextAlign {
    START = 1,
    END,
    LEFT,
    CENTER,
    RIGHT
  };

  class Context {
  public:
    Context(unsigned int _width, unsigned int _height)
      : width(_width), height(_height) { }
    virtual ~Context() { }

    virtual std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const unsigned char * data) = 0;
    virtual std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) = 0;

    std::shared_ptr<Surface> createSurface(const Image & image) {
      return createSurface(image.getWidth(), image.getHeight(), image.getData());
    }

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
    virtual TextMetrics measureText(const std::string & text) = 0;
    
    void fillRect(double x, double y, double w, double h);
    void strokeRect(double x, double y, double w, double h);
    void fillText(const std::string & text, double x, double y);
    void arcTo(double x1, double y1, double x2, double y2, double radius);
    
    virtual Surface & getDefaultSurface() = 0;
    virtual const Surface & getDefaultSurface() const = 0;

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }

    virtual void drawImage(Context & other, double x, double y, double w, double h) {
      drawImage(other.getDefaultSurface(), x, y, w, h);
    }
    virtual void drawImage(Surface & img, double x, double y, double w, double h) = 0;
    
    void drawImage(const Image & img, double x, double y, double w, double h) {
      if (img.getData()) {
	auto surface = createSurface(img);
	drawImage(*surface, x, y, w, h);
      }
    }

    Style & createLinearGradient(double x0, double y0, double x1, double y1) {
      current_linear_gradient.setType(Style::LINEAR_GRADIENT);
      current_linear_gradient.setVector(x0, y0, x1, y1);
      return current_linear_gradient;
    }    

    float lineWidth = 1.0f;
    Style fillStyle;
    Style strokeStyle;
    float shadowBlur = 0.0f;
    Color shadowColor;
    float shadowOffsetX = 0.0f, shadowOffsetY = 0.0f;
    float globalAlpha = 1.0f;
    Font font;
    TextBaseline textBaseline;
    TextAlign textAlign = LEFT;
    
  protected:
    virtual Point getCurrentPoint() = 0;
    bool hasShadow() const { return shadowBlur > 0 || shadowOffsetX != 0 || shadowOffsetY != 0; }

  private:
    unsigned int width, height;
    Style current_linear_gradient;
    
    Context(const Context & other) { }
    Context & operator=(const Context & other) { return *this; }
  };

  class ContextFactory {
  public:
    ContextFactory() { }
    virtual ~ContextFactory() { }
    virtual std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) = 0;
  };
};

#endif
