#include "Context.h"

#define NOMINMAX
#include <algorithm>
namespace Gdiplus
{
  using std::min;
  using std::max;
};

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gdiplus.h>

#include <iostream>
#include <vector>

#pragma comment (lib, "gdiplus.lib")

namespace canvas {
  class GDIPlusSurface : public Surface {
  public:
    friend class ContextGDIPlus;

  GDIPlusSurface(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat image_format)
    : Surface(_actual_width, _actual_height, _logical_width, _logical_height, image_format) {
      if (_actual_width && _actual_height) {
	bitmap = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_actual_width, _actual_height, image_format.hasAlpha() ? PixelFormat32bppPARGB : PixelFormat32bppRGB));
      }
    }
    GDIPlusSurface(const std::string & filename);
	GDIPlusSurface(const unsigned char * buffer, size_t size);
  GDIPlusSurface(const Image & image) : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getFormat().hasAlpha())
    {
      // stride must be a multiple of four
      size_t numPixels = image.getWidth() * image.getHeight();
      storage = new BYTE[4 * numPixels];
      if (image.getFormat().hasAlpha() || image.getFormat().getBytesPerPixel() == 4) {
#if 0
	for (unsigned int i = 0; i < numPixels; i++) {
	  storage[4 * i + 0] = image.getData()[4 * i + 0];
	  storage[4 * i + 1] = image.getData()[4 * i + 1];
	  storage[4 * i + 2] = image.getData()[4 * i + 2];
	  storage[4 * i + 3] = image.getData()[4 * i + 3];
	}
#else
	memcpy(storage, image.getData(), 4 * numPixels);
#endif
      } else {
  	for (unsigned int i = 0; i < numPixels; i++) {
	  storage[4 * i + 0] = image.getData()[3 * i + 2];
	  storage[4 * i + 1] = image.getData()[3 * i + 1];
	  storage[4 * i + 2] = image.getData()[3 * i + 0];
	  storage[4 * i + 3] = 255;
	}
      }
      bitmap = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(image.getWidth(), image.getHeight(), image.getWidth() * 4, image.getFormat().hasAlpha() ? PixelFormat32bppPARGB : PixelFormat32bppRGB, storage));
      // can the storage be freed here?
    }
    ~GDIPlusSurface() {
      delete[] storage;
    }
    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool _has_alpha) {
      Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _has_alpha);
      bitmap = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_actual_width, _actual_height, _has_alpha ? PixelFormat32bppPARGB : PixelFormat32bppRGB ));
      g = std::unique_ptr<Gdiplus::Graphics>(0);
    }
    void flush() { }
    void markDirty() { }

    void * lockMemory(bool write_access = false) {
      if (bitmap.get()) {
	flush();
	Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | (write_access ? Gdiplus::ImageLockModeWrite : 0), hasAlpha() ? PixelFormat32bppPARGB : PixelFormat32bppRGB, &data);
	return data.Scan0;
      } else {
	return 0;
      }
    }

#if 0
    void * lockMemoryPartial(unsigned int x0, unsigned int y0, unsigned int required_width, unsigned int required_height) {
      flush();
      Gdiplus::Rect rect(x0, y0, required_width, required_height);
      bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB, &data);
      return data.Scan0;
    }
#endif
      
    void releaseMemory() {
      Surface::releaseMemory();
      bitmap->UnlockBits(&data);
      markDirty();
    }

    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, const Point & p,, float lineWidth, Operator op, float display_scale, float globalAlpha) override;
    TextMetrics measureText(const Font & font, const std::string & text, float display_scale);

    void renderPath(RenderMode mode, const Path2D & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha) override;
    
    void drawImage(Surface & _img, const Point & p, double w, double h, float displayScale = 1.0f, float globalAlpha = 1.0f, bool imageSmoothingEnabled = true);
    void drawImage(const Image & _img, const Point & p, double w, double h, float displayScale = 1.0f, float globalAlpha = 1.0f, bool imageSmoothingEnabled = true) {
      GDIPlusSurface cs(_img);
      drawNativeSurface(cs, p, w, h, displayScale, globalAlpha, imageSmoothingEnabled);
    }
    void clip(const Path2D & path, float display_scale);
    void resetClip() override { }
    void save() {
      initializeContext();
      save_stack.push_back(g->Save());
    }
    void restore() {
      initializeContext();
      if (!save_stack.empty()) {
	g->Restore(save_stack.back());
	save_stack.pop_back();
      }
    }

  protected:
    void initializeContext() {
      if (!g.get()) {
	if (!bitmap.get()) {
	  bitmap = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(4, 4, PixelFormat32bppPARGB));
	}
	g = std::unique_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
#if 0
	g->SetPixelOffsetMode( PixelOffsetModeNone );
#endif
	g->SetCompositingQuality( Gdiplus::CompositingQualityHighQuality );
	g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
      }
    }
    void drawNativeSurface(GDIPlusSurface & img, const Point & p, double w, double h, float displayScale, float globalAlpha, bool imageSmoothingEnabled);

  private:
    std::unique_ptr<Gdiplus::Bitmap> bitmap;
    std::unique_ptr<Gdiplus::Graphics> g;
    Gdiplus::BitmapData data;     
    std::vector<Gdiplus::GraphicsState> save_stack;
    BYTE * storage = 0;
  };
  
  class ContextGDIPlus : public Context {
  public:
    ContextGDIPlus(unsigned int _width, unsigned int _height, InternalFormat _image_format, float _display_scale = 1.0f)
      : Context(_display_scale),
	default_surface(_width, _height, (unsigned int)(_display_scale * _width), (unsigned int)(_display_scale * _height), _image_format)
    {
    
    }

    static void initialize() {
      if (!is_initialized) {
	// Initialize GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	is_initialized = true;
      }
    }

    std::unique_ptr<Surface> createSurface(const Image & image) {
      return std::unique_ptr<Surface>(new GDIPlusSurface(image));
    }
    std::unique_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, InternalFormat image_format) {
      return std::unique_ptr<Surface>(new GDIPlusSurface(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), image_format));
    }

    GDIPlusSurface & getDefaultSurface() { return default_surface; }
    const GDIPlusSurface & getDefaultSurface() const { return default_surface; }

  private:   
    GDIPlusSurface default_surface;
    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;
  };

  class GDIPlusContextFactory : public ContextFactory  {
  public:
   GDIPlusContextFactory() : ContextFactory(1.0f) { }
    std::unique_ptr<Context> createContext(unsigned int width, unsigned int height, InternalFormat image_format) override {
      return std::make_unique<ContextGDIPlus>(width, height, image_format, getDisplayScale()));
    }
    std::unique_ptr<Surface> createSurface(unsigned int width, unsigned int height, InternalFormat image_format) override {
      unsigned int aw = width * getDisplayScale() : width, ah = height * getDisplayScale();
      return std::make_unique<GDIPlusSurface>(width, height, aw, ah, image_format);
    }
    std::unique_ptr<Image> loadImage(const std::string & filename) {
      return std::make_unique<Image>(filename, getDisplayScale());
    }
  };
};
