#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>
#include <memory>

#include "Color.h"
#include "Surface.h"
#include "Image.h"
#include "HitRegion.h"

namespace canvas {
  class Context;
  
  class SavedContext {
  public:
    friend class Context;
    SavedContext(const Context & context);
 
  private:
    float globalAlpha;
    bool imageSmoothingEnabled;
    float shadowBlur;
    Color shadowColor;
    float shadowOffsetX, shadowOffsetY;
    Path currentPath;
    float lineWidth;
    Style fillStyle, strokeStyle;
    Font font;
    TextAlign textAlign;
    TextBaseline textBaseline;
  };
  
  class Context {
  public:
    Context(float _display_scale = 1.0f) : display_scale(_display_scale) { }
    Context(const Context & other) = delete;
    Context & operator=(const Context & other) = delete;
    virtual ~Context() { }

    virtual std::shared_ptr<Surface> createSurface(const Image & image) = 0;
    virtual std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const ImageFormat & _format) = 0;
    virtual std::shared_ptr<Surface> createSurface(const std::string & filename) = 0;
    virtual void resize(unsigned int _width, unsigned int _height);
        
    Context & beginPath() { currentPath.clear(); return *this; }
    Context & closePath() { currentPath.closePath(); return *this; }
    Context & arc(double x, double y, double r, double a0, double a1, bool t = false) { currentPath.arc(x, y, r, a0, a1, t); return *this; }
    Context & moveTo(double x, double y) { currentPath.moveTo(x, y); return *this; }
    Context & lineTo(double x, double y) { currentPath.lineTo(x, y); return *this; }
    Context & arcTo(double x1, double y1, double x2, double y2, double radius) { currentPath.arcTo(x1, y1, x2, y2, radius); return *this; }
    Context & rect(double x, double y, double w, double h) { currentPath.rect(x, y, w, h); return *this; }
    Context & resetClip() { getDefaultSurface().resetClip(); return *this; }
    Context & stroke() { return renderPath(STROKE, currentPath, strokeStyle); }
    Context & stroke(const Path & path) { return renderPath(STROKE, path, strokeStyle); }
    Context & fill() { return renderPath(FILL, currentPath, fillStyle); }
    Context & fill(const Path & path) { return renderPath(FILL, path, fillStyle); }
    Context & save();
    Context & restore();

    Context & clip() {
      getDefaultSurface().clip(currentPath, getDisplayScale());
      currentPath.clear();
      return *this;
    }
    
    bool isPointInPath(const Path & path, double x, double y) { return false; }
    
    TextMetrics measureText(const std::string & text) {
      return getDefaultSurface().measureText(font, text, getDisplayScale());
    }
    
    Context & fillRect(double x, double y, double w, double h);
    Context & strokeRect(double x, double y, double w, double h);
    Context & clearRect(double x, double y, double w, double h);
    Context & fillText(const std::string & text, double x, double y) { return renderText(FILL, fillStyle, text, x, y); }
    Context & strokeText(const std::string & text, double x, double y) { return renderText(STROKE, strokeStyle, text, x, y); }
    
    virtual Surface & getDefaultSurface() = 0;
    virtual const Surface & getDefaultSurface() const = 0;

    unsigned int getWidth() const { return getDefaultSurface().getLogicalWidth(); }
    unsigned int getHeight() const { return getDefaultSurface().getLogicalHeight(); }
    unsigned int getActualWidth() const { return getDefaultSurface().getActualWidth(); }
    unsigned int getActualHeight() const { return getDefaultSurface().getActualHeight(); }

    Context & drawImage(Context & other, double x, double y, double w, double h) {
      return drawImage(other.getDefaultSurface(), x, y, w, h);
    }
    virtual Context & drawImage(const Image & img, double x, double y, double w, double h) {
      if (img.getData()) {
	auto surface = createSurface(img);
	return drawImage(*surface, x, y, w, h);
      } else {
	return *this;
      }
    }
    virtual Context & drawImage(Surface & img, double x, double y, double w, double h);
        
    Style & createLinearGradient(double x0, double y0, double x1, double y1) {
      current_linear_gradient.setType(Style::LINEAR_GRADIENT);
      current_linear_gradient.setVector(x0, y0, x1, y1);
      return current_linear_gradient;
    }

    float getDisplayScale() const { return display_scale; }
    Context & addHitRegion(const std::string & id, const std::string & cursor) {
      if (!currentPath.empty()) {
	HitRegion hr(id, currentPath, cursor);
	hit_regions.push_back(hr);
      }
      return *this;
    }

#if 0
    Style & createPattern(const Image & image, const char * repeat) {
      
    }
    Style & createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1) {
     }
#endif

    float lineWidth = 1.0f;
    Style fillStyle;
    Style strokeStyle;
    float shadowBlur = 0.0f;
    Color shadowColor;
    float shadowOffsetX = 0.0f, shadowOffsetY = 0.0f;
    float globalAlpha = 1.0f;
    Font font;
    TextBaseline textBaseline;
    TextAlign textAlign;
    bool imageSmoothingEnabled = true;
    Path currentPath;
    
  protected:
    virtual Context & renderPath(RenderMode mode, const Path & path, const Style & style, Operator op = SOURCE_OVER);
    virtual Context & renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op = SOURCE_OVER);

    bool hasShadow() const { return shadowBlur > 0.0f || shadowOffsetX != 0 || shadowOffsetY != 0; }
    
  private:
    Style current_linear_gradient;
    std::vector<SavedContext> restore_stack;
    float display_scale;
    std::vector<HitRegion> hit_regions;
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
    virtual std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling) = 0;
    virtual std::shared_ptr<Surface> createSurface(const std::string & filename) = 0;
    virtual std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling) = 0;
    virtual std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) = 0;

    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, bool apply_scaling) {
      return createContext(width, height, ImageFormat::RGBA32, apply_scaling);
    }

    std::shared_ptr<Image> createImage(const std::string & filename) {
      auto surface = createSurface(filename);
      return surface->createImage();
    }
    
    float getDisplayScale() const { return display_scale; }
    
  private:
    float display_scale;
  };
};

#endif
