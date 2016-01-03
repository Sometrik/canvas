#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>
#include <memory>

#include "InternalFormat.h"
#include "Color.h"
#include "Surface.h"
#include "Image.h"
#include "HitRegion.h"
#include "GraphicsState.h"

namespace canvas {
  class Context : public GraphicsState {
  public:
    Context(float _display_scale = 1.0f)
      : display_scale(_display_scale),
      current_linear_gradient(this)
      { }
    Context(const Context & other) = delete;
    Context & operator=(const Context & other) = delete;
    Context & operator=(const GraphicsState & other) {
      GraphicsState::operator=(other);
      return *this;
    }
    virtual ~Context() { }

    virtual std::shared_ptr<Surface> createSurface(const Image & image) = 0;
    virtual std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const InternalFormat & _format) = 0;
    virtual std::shared_ptr<Surface> createSurface(const std::string & filename) = 0;
    virtual void resize(unsigned int _width, unsigned int _height);
        
    Context & stroke() { return renderPath(STROKE, currentPath, strokeStyle); }
    Context & stroke(const Path2D & path) { return renderPath(STROKE, path, strokeStyle); }
    Context & fill() { return renderPath(FILL, currentPath, fillStyle); }
    Context & fill(const Path2D & path) { return renderPath(FILL, path, fillStyle); }
    Context & save();
    Context & restore();
    
    bool isPointInPath(const Path2D & path, double x, double y) { return false; }
    
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
    virtual Context & drawImage(const Image & img, double x, double y, double w, double h);
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
    const HitRegion & getHitRegion(float x, float y) const {
      for (auto & r : hit_regions) {
	if (r.isInside(x, y)) return r;
      }
      return null_region;
    }
    const std::vector<HitRegion> & getHitRegions() const { return hit_regions; }
    
#if 0
    Style & createPattern(const Image & image, const char * repeat) {
      
    }
    Style & createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1) {
     }
#endif
    
  protected:
    Context & renderPath(RenderMode mode, const Path2D & path, const Style & style, Operator op = SOURCE_OVER);
    Context & renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op = SOURCE_OVER);
    virtual bool hasNativeShadows() const { return false; }

    bool hasShadow() const { return shadowBlur.getValue() > 0.0f || shadowOffsetX.getValue() != 0 || shadowOffsetY.getValue() != 0; }
    
  private:
    float display_scale;
    Style current_linear_gradient;
    std::vector<GraphicsState> restore_stack;
    std::vector<HitRegion> hit_regions;
    HitRegion null_region;
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
    virtual std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, const InternalFormat & format, bool apply_scaling) = 0;
    virtual std::shared_ptr<Surface> createSurface(const std::string & filename) = 0;
    virtual std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, const InternalFormat & format, bool apply_scaling) = 0;
    virtual std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) = 0;

    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, bool apply_scaling) {
      return createContext(width, height, RGBA8, apply_scaling);
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
