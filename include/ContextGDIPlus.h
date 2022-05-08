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

    static inline Gdiplus::PixelFormat getPixelFormat(int num_channels) {
      switch (num_channels) {
      case 1: return PixelFormat8bppIndexed;
      case 3: return PixelFormat32bppRGB;
      case 4: return PixelFormat32bppPARGB;
      }
      return PixelFormat32bppPARGB;
    }
    
    GDIPlusSurface(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, unsigned int _num_channels)
      : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _num_channels) {
      if (_actual_width && _actual_height) {
	bitmap = std::make_unique<Gdiplus::Bitmap>(_actual_width, _actual_height, getPixelFormat(_num_channels));
      }
    }
    GDIPlusSurface(const std::string & filename);
    GDIPlusSurface(const unsigned char * buffer, size_t size);
    GDIPlusSurface(const ImageData & image) : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getNumChannels())
    {
      // stride must be a multiple of four
      size_t numPixels = image.getWidth() * image.getHeight();
      storage = new BYTE[4 * numPixels];
      if (image.getNumChannels() == 4) {
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
      bitmap = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(image.getWidth(), image.getHeight(), image.getWidth() * 4, getPixelFormat(image.getNumChannels()), storage));
      // can the storage be freed here?
    }
    ~GDIPlusSurface() {
      delete[] storage;
    }
    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, unsigned int _num_channels) override {
      Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _num_channels);
      bitmap = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_actual_width, _actual_height, getPixelFormat(_num_channels)));
      g = std::unique_ptr<Gdiplus::Graphics>(nullptr);
    }

    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, const Point & p, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) override;
    TextMetrics measureText(const Font& font, const std::string& text, TextBaseline textBaseline, float display_scale) override;
    
    void renderPath(RenderMode mode, const Path2D & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color& shadowColor, const Path2D& clipPath) override;
    
    void drawImage(Surface& _img, const Point& p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color& shadowColor, const Path2D& clipPath, bool imageSmoothingEnabled = true) override {
      GDIPlusSurface* img = dynamic_cast<GDIPlusSurface*>(&_img);
      if (img) {
	drawNativeSurface(*img, p, w, h, displayScale, globalAlpha, imageSmoothingEnabled);
      } else {
	auto img2 = _img.createImage(displayScale);
	GDIPlusSurface cs(img2->getData());
	drawNativeSurface(cs, p, w, h, displayScale, globalAlpha, imageSmoothingEnabled);
      }
    }

    void drawImage(const ImageData & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color& shadowColor, const Path2D& clipPath, bool imageSmoothingEnabled = true) override {
      GDIPlusSurface cs(_img);
      drawNativeSurface(cs, p, w, h, displayScale, globalAlpha, imageSmoothingEnabled);
    }
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

    std::unique_ptr<Image> createImage(float display_scale) override;
    
  protected:
    void * lockMemory(bool write_access = false) override {
      if (bitmap.get()) {
	Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | (write_access ? Gdiplus::ImageLockModeWrite : 0), getPixelFormat(getNumChannels()), &data);
	return data.Scan0;
      } else {
	return 0;
      }
    }
      
    void releaseMemory() override {
      if (bitmap) bitmap->UnlockBits(&data);
    }

    bool initializeContext() {
      if (!g) {
	if (!bitmap) {
	  bitmap = std::make_unique<Gdiplus::Bitmap>(16, 16, getPixelFormat(4));
	}
	g = std::make_unique<Gdiplus::Graphics>(bitmap.get());
	if (g) {
#if 0
	  g->SetPixelOffsetMode(PixelOffsetModeNone);
#endif
	  g->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	  g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	}
      }
      return g.get() != nullptr;
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
    ContextGDIPlus(unsigned int _width, unsigned int _height, unsigned int _num_channels = 4, float _display_scale = 1.0f)
      : Context(_display_scale),
	default_surface(_width, _height, (unsigned int)(_display_scale * _width), (unsigned int)(_display_scale * _height), _num_channels)
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

    std::unique_ptr<Surface> createSurface(const ImageData & image) override {
      return std::unique_ptr<Surface>(new GDIPlusSurface(image));
    }
    std::unique_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, unsigned int num_channels) override {
      return std::unique_ptr<Surface>(new GDIPlusSurface(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), num_channels));
    }

    GDIPlusSurface & getDefaultSurface() override { return default_surface; }
    const GDIPlusSurface & getDefaultSurface() const override { return default_surface; }

  private:   
    GDIPlusSurface default_surface;
    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;
  };

  class GDIPlusContextFactory : public ContextFactory  {
  public:
    GDIPlusContextFactory(float display_scale) : ContextFactory(display_scale) {
      ContextGDIPlus::initialize();
    }
    std::unique_ptr<Context> createContext(unsigned int width, unsigned int height, unsigned int num_channels = 4) override {
      return std::make_unique<ContextGDIPlus>(width, height, num_channels, getDisplayScale());
    }
    std::unique_ptr<Surface> createSurface(unsigned int width, unsigned int height, unsigned int num_channels = 4) override {
      unsigned int aw = width * getDisplayScale(), ah = height * getDisplayScale();
      return std::make_unique<GDIPlusSurface>(width, height, aw, ah, num_channels);
    }
    std::unique_ptr<Image> loadImage(const std::string & filename) override;
    std::unique_ptr<Image> createImage() override;
    std::unique_ptr<Image> createImage(const unsigned char * _data, unsigned int _width, unsigned int _height, unsigned int _num_channels) override;
  };
};
