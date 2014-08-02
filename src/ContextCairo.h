#include "Context.h"

#include <cairo/cairo.h>
// #include <pango/pangocairo.h>

#include <iostream>
#include <cassert>

namespace canvas {
  class CairoSurface : public Surface {
  public:
    // friend class ContextCairo;

    CairoSurface(unsigned int _width, unsigned int _height);
    CairoSurface(unsigned int _width, unsigned int _height, unsigned char * data);
    ~CairoSurface();

    void resize(unsigned int width, unsigned int height);

    unsigned char * getBuffer();
    const unsigned char * getBuffer() const;

    // cairo_image_surface_get_stride(surface);

    cairo_surface_t * surface;
  protected:
  };

  class ContextCairo : public Context {
  public:
    ContextCairo(unsigned int _width = 0, unsigned int _height = 0);
    ~ContextCairo();

    void check() const;

    void save();
    void restore();

#if 0
    CairoSurface & getDefaultSurface() {
      std::cerr << "trying to get default surface, this = " << this << ", s = " << &default_surface << std::endl;
      return default_surface;
    }
    const CairoSurface & getDefaultSurface() const {
      std::cerr << "trying to get default surface (2), this = " << this << ", s = " << &default_surface << std::endl;
      return default_surface;
    }
#endif
    
    void resize(unsigned int width, unsigned int height);
    
    void flush() {
      // std::cerr << "flushing\n";
      // gc->Flush();
      // delete gc;
      // gc = 0;      
    }

    void beginPath() {
      // path.CloseSubpath();
    }
    void closePath() {
      // path.CloseSubpath();    
    }
    void clip();
    void arc(double x, double y, double r, double a0, double a1, bool t = false);
    void clearRect(double x, double y, double w, double h) { }
    void moveTo(double x, double y);
    void lineTo(double x, double y);
    void stroke();
    void fill();
    void fillText(const std::string & text, double x, double y);
    Size measureText(const std::string & text);

    void drawImage(Context & other, double x, double y, double w, double h) {
      Context::drawImage(other, x, y, w, h);
    }
    void drawImage(Surface & img, double x, double y, double w, double h);

  protected:
    cairo_t * cr;  
#if 0
    CairoSurface default_surface;
#endif
  
  private:
    // PangoFontDescription * font_description;
    
#if 0
    static Mutex pango_mutex;
    static Mutex draw_mutex;
#endif
  };
};
