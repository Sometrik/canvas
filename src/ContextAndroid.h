#ifndef _CANVAS_CONTEXTANDROID_H_
#define _CANVAS_CONTEXTANDROID_H_

#include "Context.h"

#include <cassert>
#include <QuartzCore/QuartzCore.h>
#include <CoreText/CoreText.h>

#include <sstream>

namespace canvas { 
  class AndroidSurface : public Surface {
  public:
    friend class ContextAndroid;
        
  AndroidSurface(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, const ImageFormat & _format)
    : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format.hasAlpha()) {

  }
  
  AndroidSurface(const Image & image)
    : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getFormat().hasAlpha()) {

    }
    
    AndroidSurface(const std::string & filename) {

    }
    
    AndroidSurface(const unsigned char * buffer, size_t size) {

    }
    
    ~AndroidSurface() {
     
    }

    void * lockMemory(bool write_access = false) override {
      return 0;
    }
    
    void releaseMemory() override { }

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale) override {

    }

    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool _has_alpha) override {
      Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _has_alpha);
      
    }
    
    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, Operator op, float display_scale) override {

    }

    TextMetrics measureText(const Font & font, const std::string & text, float display_scale) override {
      return TextMetrics(0);
    }

    void drawImage(Surface & surface, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) override {
    }
    
    void drawImage(const Image & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) override {
    
    }
    void clip(const Path & path, float display_scale) override {
    
    }
    void resetClip() override {
    }
    void save() override {
    }
    void restore() override {
    }

  protected:    
  private:
  };

  class ContextAndroid : public Context {
  public:
  ContextAndroid(unsigned int _width, unsigned int _height, const ImageFormat & format, float _display_scale)
    : Context(_display_scale),
      default_surface(_width, _height, (unsigned int)(_width * _display_scale), (unsigned int)(_height * _display_scale), format)
      {
      }

    std::shared_ptr<Surface> createSurface(const Image & image) override {
      return std::shared_ptr<Surface>(new AndroidSurface(image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const ImageFormat & _format) override {
      return std::shared_ptr<Surface>(new AndroidSurface(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), _format));
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      return std::shared_ptr<Surface>(new AndroidSurface(filename));
    }
        
    Surface & getDefaultSurface() override { return default_surface; }
    const Surface & getDefaultSurface() const override { return default_surface; }
      
    void drawImage(const Image & img, double x, double y, double w, double h) override {

    }
    
    void drawImage(Surface & img, double x, double y, double w, double h) override {

    }
    
  protected:
    void renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op) override {

    }
    
    void renderPath(RenderMode mode, const Path & path, const Style & style, Operator op) override {

    }
    
  private:
    AndroidSurface default_surface;
  };

  class AndroidContextFactory : public ContextFactory {
  public:
    AndroidContextFactory(float _display_scale) : ContextFactory(_display_scale), converter(_converter) { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling) override {
      std::shared_ptr<Context> ptr(new ContextAndroid(width, height, format, apply_scaling ? getDisplayScale() : 1.0f));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      return std::shared_ptr<Surface> ptr(new AndroidSurface(filename));
    }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling) override {
      unsigned int aw = apply_scaling ? width * getDisplayScale() : width;
      unsigned int ah = apply_scaling ? height * getDisplayScale() : height;
      std::shared_ptr<Surface> ptr(new AndroidSurface(width, height, aw, ah, format));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) override {
      std::shared_ptr<Surface> ptr(new AndroidSurface(buffer, size));
      return ptr;
    }
    
  private:
  };
};

#endif
