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

    GDIPlusSurface(unsigned int _width, unsigned int _height) : Surface(_width, _height), 
      bitmap(new Gdiplus::Bitmap(_width, _height, PixelFormat32bppPARGB)),
      g(new Gdiplus::Graphics(&(*bitmap)))
    {
      initialize();
    }
    GDIPlusSurface(const std::string & filename);
    GDIPlusSurface(const Image & image) : Surface(image.getWidth(), image.getHeight())
    {
      BYTE * tmp = new BYTE[(image.hasAlpha() ? 4 : 3) * image.getWidth() * image.getHeight()]; // FIXME Free?
      if (image.hasAlpha()) {
	for (unsigned int i = 0; i < image.getWidth() * image.getHeight(); i++) {
	  tmp[4 * i + 0] = image.getData()[4 * i + 0];
	  tmp[4 * i + 1] = image.getData()[4 * i + 1];
	  tmp[4 * i + 2] = image.getData()[4 * i + 2];
	  tmp[4 * i + 3] = image.getData()[4 * i + 3];
	}
      } else {
	for (unsigned int i = 0; i < image.getWidth() * image.getHeight(); i++) {
	  tmp[3 * i + 0] = image.getData()[3 * i + 2];
	  tmp[3 * i + 1] = image.getData()[3 * i + 1];
	  tmp[3 * i + 2] = image.getData()[3 * i + 0];
	}
      }
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(image.getWidth(), image.getHeight(), image.getWidth() * (image.hasAlpha() ? 4 : 3), image.hasAlpha() ? PixelFormat32bppPARGB : PixelFormat24bppRGB, tmp));
      delete[] tmp;
      g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
      initialize();
    }
    ~GDIPlusSurface() {
    }
    GDIPlusSurface * copy() {
      return 0;
    }
    void resize(unsigned int _width, unsigned int _height) {
      Surface::resize(_width, _height);
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_width, _height));
      g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
      initialize();
    }
    void initialize() {
      g->SetCompositingMode( Gdiplus::CompositingModeSourceOver );
#if 0
      g->SetPixelOffsetMode( PixelOffsetModeNone );
#endif
      g->SetCompositingQuality( Gdiplus::CompositingQualityHighQuality );
      g->SetInterpolationMode( Gdiplus::InterpolationModeHighQualityBicubic );
      g->SetCompositingQuality( Gdiplus::CompositingQualityHighQuality );
      g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
#if 0
      g->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
#endif
    }
    void flush() { }
    void markDirty() { }

    unsigned char * lockMemory(bool write_access = false, unsigned int required_width = 0, unsigned int required_height = 0) {
      flush();
      Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
      bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | (write_access ? Gdiplus::ImageLockModeWrite : 0), PixelFormat32bppPARGB, &data);
      return (unsigned char*)data.Scan0;
    }
      
    void releaseMemory() {
      bitmap->UnlockBits(&data);
      markDirty();
    }

    void fillText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y);
    
    void strokeText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) {
      
    }  

    void drawImage(Surface & _img, double x, double y, double w, double h, float alpha = 1.0f);
    void clip(const Path & path);
    void stroke(const Path & path, const Style & style, double lineWidth);
    void fill(const Path & path, const Style & style);
    void save() {
      save_stack.push_back(g->Save());
    }
    void restore() {
      assert(!save_stack.empty());
      if (!save_stack.empty()) {
	g->Restore(save_stack.back());
	save_stack.pop_back();
      }
    }

  protected:
    void drawNativeSurface(GDIPlusSurface & img, double x, double y, double w, double h, double alpha);

  private:
    std::shared_ptr<Gdiplus::Bitmap> bitmap;
    std::shared_ptr<Gdiplus::Graphics> g;
    Gdiplus::BitmapData data;     
    std::vector<Gdiplus::GraphicsState> save_stack;
  };
  
  class ContextGDIPlus : public Context {
  public:
    ContextGDIPlus(unsigned int _width = 0, unsigned int _height = 0)
      : Context(_width, _height),
	default_surface(_width, _height)
    {
    
    }

    ~ContextGDIPlus() {
      
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
    
    void clearRect(double x, double y, double w, double h) { }
    TextMetrics measureText(const std::string & text);
    
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
