#include "Context.h"

#include <cairo/cairo.h>
// #include <pango/pangocairo.h>

#include <iostream>
#include <cassert>

namespace canvas {
  class ContextCairo;

  class CairoSurface : public Surface {
  public:
    friend class ContextCairo;

    CairoSurface(unsigned int _width, unsigned int _height);
    CairoSurface(unsigned int _width, unsigned int _height, const unsigned char * data);
    ~CairoSurface();

    void flush();
    void markDirty();
    unsigned char * lockMemory(bool write_access) {
      flush();
      locked_for_write = write_access;
      return cairo_image_surface_get_data(surface);
    }
    void releaseMemory() {
      if (locked_for_write) {
	locked_for_write = false;
	markDirty();
      }
    }
    void resize(unsigned int width, unsigned int height);
    
  protected:
    void fillText(Context & context, const std::string & text, double x, double y);
    void drawImage(Surface & _img, double x, double y, double w, double h);

    cairo_t * cr;  
    cairo_surface_t * surface;
    unsigned int * storage = 0;
    bool locked_for_write = false;
  };

  class ContextCairo : public Context {
  public:
    ContextCairo(unsigned int _width = 0, unsigned int _height = 0);
    ~ContextCairo();

    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const unsigned char * _data) {
      return std::shared_ptr<Surface>(new CairoSurface(_width, _height, _data));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new CairoSurface(_width, _height));
    }

    void save();
    void restore();

    CairoSurface & getDefaultSurface() { return default_surface; }
    const CairoSurface & getDefaultSurface() const { return default_surface; }
        
    void beginPath();
    void closePath();
    void clip();
    void arc(double x, double y, double r, double a0, double a1, bool t = false);
    void clearRect(double x, double y, double w, double h) { }
    void moveTo(double x, double y);
    void lineTo(double x, double y);
    void stroke();
    void fill();
    TextMetrics measureText(const std::string & text);

  protected:
    Point getCurrentPoint();

    CairoSurface default_surface;
  
  private:
    // PangoFontDescription * font_description;
    
#if 0
    static Mutex pango_mutex;
    static Mutex draw_mutex;
#endif
  };

  class CairoContextFactory : public ContextFactory {
  public:
    CairoContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) const { return std::shared_ptr<Context>(new ContextCairo(width, height)); }
  };
};
