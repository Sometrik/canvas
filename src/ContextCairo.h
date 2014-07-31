#include "Context.h"

#include <cairo/cairo.h>
// #include <pango/pangocairo.h>

#include <iostream>
#include <cassert>

namespace canvas {
  class CairoSurface : public Surface {
  public:
    friend class ContextCairo;

    CairoSurface(unsigned int width, unsigned int height) {
      cairo_format_t format = CAIRO_FORMAT_ARGB32;
      // format = CAIRO_FORMAT_RGB24;
      surface = cairo_image_surface_create(format, width, height);
      assert(surface);
      std::cerr << "created surface " << width << " " << height << " " << surface << " this = " << this << std::endl;
    }
    CairoSurface(unsigned int width, unsigned int height, unsigned char * data) {
      cairo_format_t format = CAIRO_FORMAT_RGB24;
      surface = cairo_image_surface_create_for_data(data,
						    format,
						    width,
						    height,
						    width);
      assert(surface);
    }
    ~CairoSurface() {
      cairo_surface_destroy(surface);
    }    

    void resize(unsigned int width, unsigned int height) {
      if (surface) {
	cairo_surface_destroy(surface);
      }
      cairo_format_t format = CAIRO_FORMAT_ARGB32;
      // format = CAIRO_FORMAT_RGB24;
      surface = cairo_image_surface_create(format, width, height);
      assert(surface);
      std::cerr << "recreated surface " << width << " " << height << " " << surface << " this = " << this << std::endl;
    }

    unsigned char * getBuffer() {
      assert(surface);
      return cairo_image_surface_get_data(surface);
    }

    const unsigned char * getBuffer() const {
      assert(surface);
      return cairo_image_surface_get_data(surface);
    }

    unsigned int getWidth() const {
      std::cerr << "getWidth() " << surface << std::endl;
      assert(surface);
      return cairo_image_surface_get_width(surface);
    }
    
    unsigned int getHeight() const {
      std::cerr << "getHeight() " << surface << std::endl;
      assert(surface);
      return cairo_image_surface_get_height(surface);
    }

    // cairo_image_surface_get_stride(surface);

  protected:
    cairo_surface_t * surface;
  };

  class ContextCairo : public Context {
  public:
    ContextCairo(unsigned int width = 0, unsigned int height = 0);
    ~ContextCairo();

    void check() const;

    void save();
    void restore();

    Surface & getDefaultSurface() {
      std::cerr << "trying to get default surface, this = " << this << std::endl;
      return default_surface;
    }
    const Surface & getDefaultSurface() const {
      std::cerr << "trying to get default surface (2), this = " << this << std::endl;
      return default_surface;
    }
    
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
    CairoSurface default_surface;
  
  private:
    // PangoFontDescription * font_description;
    
#if 0
    static Mutex pango_mutex;
    static Mutex draw_mutex;
#endif
  };
};
