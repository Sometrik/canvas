#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>
#include <memory>

#include "Color.h"
#include "Surface.h"
#include "Image.h"
#include "TextMetrics.h"

namespace canvas {  
  class SavedContext {
  public:
    friend class Context;
  private:
    float globalAlpha;
    Path current_path;
  };
  class Context {
  public:
    Context(unsigned int _width, unsigned int _height, float _display_scale = 1.0f)
      : width(_width), height(_height), display_scale(_display_scale) { }
    virtual ~Context() { }

    virtual std::shared_ptr<Surface> createSurface(const Image & image) = 0;
    virtual std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) = 0;
    virtual std::shared_ptr<Surface> createSurface(const std::string & filename) = 0;

    virtual void resize(unsigned int _width, unsigned int _height);
    
    virtual void clearRect(double x, double y, double w, double h) = 0;

    void beginPath() { current_path.clear(); }
    void closePath() { current_path.close(); }

    void arc(double x, double y, double r, double a0, double a1, bool t = false) { current_path.arc(x, y, r, a0, a1, t); }
    void moveTo(double x, double y) { current_path.moveTo(x, y); }
    void lineTo(double x, double y) { current_path.lineTo(x, y); }
    void arcTo(double x1, double y1, double x2, double y2, double radius) { current_path.arcTo(x1, y1, x2, y2, radius); }

    void clip() {
      getDefaultSurface().clip(current_path);
      current_path.clear();
    }
    void stroke();
    void fill();

    void save();
    void restore();

    virtual TextMetrics measureText(const std::string & text) = 0;
    
    void rect(double x, double y, double w, double h);
    void fillRect(double x, double y, double w, double h);
    void strokeRect(double x, double y, double w, double h);
    void fillText(const std::string & text, double x, double y);
    void strokeText(const std::string & text, double x, double y);
    
    virtual Surface & getDefaultSurface() = 0;
    virtual const Surface & getDefaultSurface() const = 0;

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }

    void drawImage(Context & other, double x, double y, double w, double h) {
      drawImage(other.getDefaultSurface(), x, y, w, h);
    }
    void drawImage(const Image & img, double x, double y, double w, double h) {
      if (img.getData()) {
	auto surface = createSurface(img);
	drawImage(*surface, x, y, w, h);
      }
    }
    void drawImage(Surface & img, double x, double y, double w, double h);
        
    Style & createLinearGradient(double x0, double y0, double x1, double y1) {
      current_linear_gradient.setType(Style::LINEAR_GRADIENT);
      current_linear_gradient.setVector(x0, y0, x1, y1);
      return current_linear_gradient;
    }    

#if 0
    Style & createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1) {
     }
#endif

    float lineWidth = 1.0f;
    Style fillStyle;
    Style strokeStyle;
    float shadowBlur = 0.0f, shadowBlurX = 0.0f, shadowBlurY = 0.0f;   
    Color shadowColor;
    float shadowOffsetX = 0.0f, shadowOffsetY = 0.0f;
    float globalAlpha = 1.0f;
    Font font;
    TextBaseline textBaseline;
    TextAlign textAlign;
    bool imageSmoothingEnabled = true;
    
  protected:
    bool hasShadow() const { return shadowBlur > 0 || shadowBlurX > 0 || shadowBlurY > 0 || shadowOffsetX != 0 || shadowOffsetY != 0; }
    float getDisplayScale() const { return display_scale; }
    
    Path current_path;

  private:
    Context(const Context & other) { }
    Context & operator=(const Context & other) { return *this; }

    unsigned int width, height;
    Style current_linear_gradient;
    std::vector<SavedContext> restore_stack;
    float display_scale;
  };
  
  class FilenameConverter {
  public:
    FilenameConverter() { }
    virtual ~FilenameConverter() { }
    virtual bool convert(const std::string & input, std::string & output) = 0;
  };
  
  class NullConverter {
  public:
    bool convert(const std::string & input, std::string & output) { output = input; return true; }
  };
  
  class ContextFactory {
  public:
    ContextFactory(float _display_scale = 1.0f) : display_scale(_display_scale) { }
    virtual ~ContextFactory() { }
    virtual std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) const = 0;
    virtual std::shared_ptr<Surface> createSurface(const std::string & filename) const = 0;
    virtual std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height) const = 0;
    virtual std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) const = 0;
    
    float getDisplayScale() const { return display_scale; }
    
  private:
    float display_scale;
  };
};

#endif
