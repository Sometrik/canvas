#include "Context.h"

#include <cairo/cairo.h>

namespace canvas {
  class ContextCairo;

  class CairoSurface : public Surface {
  public:
    friend class ContextCairo;

    CairoSurface(unsigned int _width, unsigned int _height, bool has_alpha = true);
    // CairoSurface(unsigned int _width, unsigned int _height, const unsigned char * data, bool has_alpha = false);
    CairoSurface(const Image & image);
    CairoSurface(const std::string & filename);
    CairoSurface(const CairoSurface & other) = delete;
    ~CairoSurface();

    CairoSurface * copy() {
      auto img = createImage();
      return new CairoSurface(*img);
    }
    
    void flush();
    void markDirty();
    void * lockMemory(bool write_access = false, unsigned int required_width = 0, unsigned int required_height = 0) {
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
    void resize(unsigned int width, unsigned int height);

    void fillText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y);
    void strokeText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y);
    void drawImage(Surface & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true);
    void clip(const Path & path);
    void stroke(const Path & path, const Style & style, double lineWidth);
    void fill(const Path & path, const Style & style);
    void save();
    void restore();
    
  protected:
    void drawNativeSurface(CairoSurface & img, double x, double y, double w, double h, float alpha, bool imageSmoothingEnabled);

    void prepareTextStyle(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign);
    void sendPath(const Path & path);

  private:
    cairo_t * cr;  
    cairo_surface_t * surface;
    unsigned int * storage = 0;
    bool locked_for_write = false;
  };

  class ContextCairo : public Context {
  public:
    ContextCairo(unsigned int _width = 0, unsigned int _height = 0);
    ~ContextCairo();

#if 0
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const unsigned char * _data) {
      return std::shared_ptr<Surface>(new CairoSurface(_width, _height, _data));
    }
#endif
    std::shared_ptr<Surface> createSurface(const Image & image) {
      return std::shared_ptr<Surface>(new CairoSurface(image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new CairoSurface(_width, _height));
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) {
      return std::shared_ptr<Surface>(new CairoSurface(filename));
    }

    CairoSurface & getDefaultSurface() { return default_surface; }
    const CairoSurface & getDefaultSurface() const { return default_surface; }
        
    void clearRect(double x, double y, double w, double h) { }

    TextMetrics measureText(const std::string & text);

  protected:
    CairoSurface default_surface;
  };

  class CairoContextFactory : public ContextFactory {
  public:
    CairoContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) const { return std::shared_ptr<Context>(new ContextCairo(width, height)); }
    std::shared_ptr<Surface> createSurface(const std::string & filename) const { return std::shared_ptr<Surface>(new CairoSurface(filename)); }
    virtual std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height) const { return std::shared_ptr<Surface>(new CairoSurface(width, height, false)); }
  };
};
