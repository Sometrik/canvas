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
#include <cassert>
#include <vector>

#pragma comment (lib, "gdiplus.lib")

namespace canvas {
  class GDIPlusSurface : public Surface {
  public:
    friend class ContextGDIPlus;

  GDIPlusSurface(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool has_alpha)
    : Surface(_width, _height) {
      if (_width && _height) {
	bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_width, _height, has_alpha ? PixelFormat32bppPARGB : PixelFormat32bppRGB));
      }
    }
    GDIPlusSurface(const std::string & filename);
  GDIPlusSurface(const Image & image) : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getFormat().hasAlpha())
    {
      // stride must be a multiple of four
      size_t numPixels = image.getWidth() * image.getHeight();
      storage = new BYTE[4 * numPixels];
      if (image.hasAlpha() || image.getBytesPerPixel() == 4) {
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
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(image.getWidth(), image.getHeight(), image.getWidth() * 4, image.hasAlpha() ? PixelFormat32bppPARGB : PixelFormat32bppRGB, storage));
      // can the storage be freed here?
    }
    ~GDIPlusSurface() {
      delete[] storage;
    }
    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool _has_alpha) {
      Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _has_alpha);
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_actual_width, _actual_height, _has_alpha ? PixelFormat32bppPARGB : PixelFormat32bppRGB ));
      g = std::shared_ptr<Gdiplus::Graphics>(0);
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

    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, float display_scale);
    TextMetrics measureText(const Font & font, const std::string & text, float display_scale);

    void renderPath(RenderMode mode, const Path & path, const Style & style, double lineWidth);
    
    void drawImage(Surface & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true);
    void clip(const Path & path);
    void save() {
      initializeContext();
      save_stack.push_back(g->Save());
    }
    void restore() {
      initializeContext();
      assert(!save_stack.empty());
      if (!save_stack.empty()) {
	g->Restore(save_stack.back());
	save_stack.pop_back();
      }
    }

  protected:
    void initializeContext() {
      if (!g.get()) {
	if (!bitmap.get()) {
	  bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(4, 4, PixelFormat32bppPARGB));
	}
	g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
	g->SetCompositingMode( Gdiplus::CompositingModeSourceOver );
#if 0
	g->SetPixelOffsetMode( PixelOffsetModeNone );
#endif
	g->SetCompositingQuality( Gdiplus::CompositingQualityHighQuality );
	g->SetCompositingQuality( Gdiplus::CompositingQualityHighQuality );
	g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
#if 0
	g->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
#endif
      }
    }
    void drawNativeSurface(GDIPlusSurface & img, double x, double y, double w, double h, double alpha, bool imageSmoothingEnabled);

  private:
    std::shared_ptr<Gdiplus::Bitmap> bitmap;
    std::shared_ptr<Gdiplus::Graphics> g;
    Gdiplus::BitmapData data;     
    std::vector<Gdiplus::GraphicsState> save_stack;
    BYTE * storage = 0;
  };
  
  class ContextGDIPlus : public Context {
  public:
    ContextGDIPlus(unsigned int _width = 0, unsigned int _height = 0)
      : Context(_width, _height),
	default_surface(_width, _height)
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

    std::shared_ptr<Surface> createSurface(const Image & image) {
      return std::shared_ptr<Surface>(new GDIPlusSurface(image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new GDIPlusSurface(_width, _height));
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) {
      return std::shared_ptr<Surface>(new GDIPlusSurface(filename));
    }

    GDIPlusSurface & getDefaultSurface() { return default_surface; }
    const GDIPlusSurface & getDefaultSurface() const { return default_surface; }
        
  protected:

  private:   
    GDIPlusSurface default_surface;
    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;
  };

  class GDIPlusContextFactory : public ContextFactory  {
  public:
    GDIPlusContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) const { return std::shared_ptr<Context>(new ContextGDIPlus(width, height)); }
    std::shared_ptr<Surface> createSurface(const std::string & filename) const { return std::shared_ptr<Surface>(new GDIPlusSurface(filename)); }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height) const { return std::shared_ptr<Surface>(new GDIPlusSurface(width, height)); }
  };
};
