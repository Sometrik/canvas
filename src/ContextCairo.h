#include "Context.h"

#include <cairo/cairo.h>

namespace canvas {
  class ContextCairo;

  class CairoSurface : public Surface {
  public:
    friend class ContextCairo;

    CairoSurface(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool has_alpha = true);
    CairoSurface(const Image & image);
    CairoSurface(const std::string & filename);
    CairoSurface(const CairoSurface & other) = delete;
    CairoSurface(const unsigned char * buffer, size_t size);
    ~CairoSurface();
    
    void flush();
    void markDirty();
    void * lockMemory(bool write_access = false) {
      flush();
      locked_for_write = write_access;
      return cairo_image_surface_get_data(surface);
    }
    void releaseMemory() {
      Surface::releaseMemory();
      if (locked_for_write) {
	locked_for_write = false;
	markDirty();
      }
    }
    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool has_alpha);

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale);
    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, Operator op, float display_scale);
    TextMetrics measureText(const Font & font, const std::string & text, float display_scale);
    void drawImage(Surface & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true);
    void drawImage(const Image & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true);
    void clip(const Path & path, float display_scale);
    void resetClip();
    void save();
    void restore();
    
  protected:
    void initializeContext() {
      if (!cr) {
	if (!surface) {
	  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 4, 4);
	}
	cr = cairo_create(surface);	
	assert(cr);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
      }
    }

    void drawNativeSurface(CairoSurface & img, double x, double y, double w, double h, float alpha, bool imageSmoothingEnabled);

    void sendPath(const Path & path);

  private:
    cairo_t * cr = 0;
    cairo_surface_t * surface;
    unsigned int * storage = 0;
    bool locked_for_write = false;
  };

  class ContextCairo : public Context {
  public:
    ContextCairo(unsigned int _width = 0, unsigned int _height = 0, float _display_scale = 1.0f)
      : Context(_display_scale),
      default_surface(_width, _height, (unsigned int)(_display_scale * _width), (unsigned int)(_display_scale * _height), true)
	{ 
	}
    
    std::shared_ptr<Surface> createSurface(const Image & image) {
      return std::shared_ptr<Surface>(new CairoSurface(image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, bool has_alpha) {
      return std::shared_ptr<Surface>(new CairoSurface(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), has_alpha));
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) {
      return std::shared_ptr<Surface>(new CairoSurface(filename));
    }

    CairoSurface & getDefaultSurface() { return default_surface; }
    const CairoSurface & getDefaultSurface() const { return default_surface; }
    
  protected:
    CairoSurface default_surface;
  };

  class CairoContextFactory : public ContextFactory {
  public:
    CairoContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, bool apply_scaling) { return std::shared_ptr<Context>(new ContextCairo(width, height)); }
    std::shared_ptr<Surface> createSurface(const std::string & filename) { return std::shared_ptr<Surface>(new CairoSurface(filename)); }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, bool has_alpha) { return std::shared_ptr<Surface>(new CairoSurface(width, height, (unsigned int)(width * getDisplayScale()), (unsigned int)(height * getDisplayScale()), has_alpha)); }
    std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) {
      std::shared_ptr<Surface> ptr(new CairoSurface(buffer, size));
      return ptr;
    }
  };
};
