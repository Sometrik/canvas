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

#include "utf8.h"

#pragma comment (lib, "gdiplus.lib")

namespace canvas {
  inline std::wstring convert_to_wstring(const std::string & input) {
    const char * str = input.c_str();
    const char * str_i = str;
    const char * end = str + input.size();
    std::wstring output;
    while (str_i < end) {
      output += (wchar_t)utf8::next(str_i, end);
    }
    return output;
  }

  class GDIPlusSurface : public Surface {
  public:
    friend class ContextGDIPlus;

    GDIPlusSurface(unsigned int _width, unsigned int _height) : Surface(_width, _height), 
      bitmap(new Gdiplus::Bitmap(_width, _height, PixelFormat32bppPARGB)),
      g(new Gdiplus::Graphics(&(*bitmap)))
    {
#if 0
      g->SetCompositingMode( CompositingModeSourceCopy );
      g->SetCompositingQuality( CompositingQualityHighSpeed );
      g->SetPixelOffsetMode( PixelOffsetModeNone );
      g->SetInterpolationMode( InterpolationModeDefault );
#endif
      g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
    }
    GDIPlusSurface(const std::string & filename) : Surface(0, 0)
     {
      std::wstring tmp = convert_to_wstring(filename);
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromFile(tmp.data()));
      Surface::resize(bitmap->GetWidth(), bitmap->GetHeight());
      g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
      g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
    }
    // Gdiplus::PixelFormat32bppARGB
    GDIPlusSurface(unsigned int _width, unsigned int _height, const unsigned char * _data) : Surface(_width, _height)
    {
      BYTE * tmp = new BYTE[3 * _width * _height]; // FIXME Free?
      for (unsigned int i = 0; i < _width * _height; i++) {
	tmp[3 * i + 0] = _data[3 * i + 2];
	tmp[3 * i + 1] = _data[3 * i + 1];
	tmp[3 * i + 2] = _data[3 * i + 0];
      }
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_width, _height, _width * 3, PixelFormat24bppRGB, tmp));
      g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
      g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
    }
    // Gdiplus::PixelFormat32bppARGB
  GDIPlusSurface(const Image & image) : Surface(image.getWidth(), image.getHeight())
    {
      BYTE * tmp = new BYTE[3 * image.getWidth() * image.getHeight()]; // FIXME Free?
      for (unsigned int i = 0; i < image.getWidth() * image.getHeight(); i++) {
	tmp[3 * i + 0] = image.getData()[3 * i + 2];
	tmp[3 * i + 1] = image.getData()[3 * i + 1];
	tmp[3 * i + 2] = image.getData()[3 * i + 0];
      }
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(image.getWidth(), image.getHeight(), image.getWidth() * 3, PixelFormat24bppRGB, tmp));
      g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
      g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
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
      g->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
    }
    void flush() { }
    void markDirty() { }

    unsigned char * lockMemory(bool write_access) {
      flush();
      Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
      bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | (write_access ? Gdiplus::ImageLockModeWrite : 0), PixelFormat32bppARGB, &data);
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

    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const unsigned char * data) {
      return std::shared_ptr<Surface>(new GDIPlusSurface(_width, _height, data));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new GDIPlusSurface(_width, _height));
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) {
      return std::shared_ptr<Surface>(new GDIPlusSurface(filename));
    }

    GDIPlusSurface & getDefaultSurface() { return default_surface; }
    const GDIPlusSurface & getDefaultSurface() const { return default_surface; }
    
    void resize(unsigned int _width, unsigned int _height) {
      Context::resize(_width, _height);
    }
    void flush() {
    }
    void clearRect(double x, double y, double w, double h) { }
    TextMetrics measureText(const std::string & text) {
      std::wstring text2 = convert_to_wstring(text);
      int style = 0;
      if (font.weight == Font::BOLD || font.weight == Font::BOLDER) {
	style |= Gdiplus::FontStyleBold;
      }
      if (font.slant == Font::ITALIC) {
	style |= Gdiplus::FontStyleItalic;
      }
      Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), font.size, style, Gdiplus::UnitPixel);
      Gdiplus::RectF layoutRect(0, 0, 512, 512), boundingBox;
      default_surface.g->MeasureString(text2.data(), text2.size(), &font, layoutRect, &boundingBox);
      Gdiplus::SizeF size;
      boundingBox.GetSize(&size);

      return { (float)size.Width, (float)size.Height };
    }
    
  protected:
    // Point getCurrentPoint() { return Point(current_position.X, current_position.Y); }

    GDIPlusSurface default_surface;

  private:   
    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;
  };

  class GDIPlusContextFactory : public ContextFactory  {
  public:
    GDIPlusContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) const { return std::shared_ptr<Context>(new ContextGDIPlus(width, height)); }
    std::shared_ptr<Surface> createSurface(const std::string & filename) const { return std::shared_ptr<Surface>(new GDIPlusSurface(filename)); }
  };
};
