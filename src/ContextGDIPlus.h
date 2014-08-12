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

    // PixelFormat32bppARGB
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
    void resize(unsigned int _width, unsigned int _height) {
      Surface::resize(_width, _height);
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_width, _height));
      g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
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

    void fillText(Context & context, const std::string & text, double x, double y) {
      std::wstring text2 = convert_to_wstring(text);
      int style = 0;
      if (context.font.weight == Font::BOLD || context.font.weight == Font::BOLDER) {
	style |= Gdiplus::FontStyleBold;
      }
      if (context.font.slant == Font::ITALIC) {
	style |= Gdiplus::FontStyleItalic;
      }
      Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), context.font.size, style, Gdiplus::UnitPixel);
      Gdiplus::SolidBrush brush(Gdiplus::Color(context.fillStyle.color.red, context.fillStyle.color.green, context.fillStyle.color.blue));

      Gdiplus::RectF rect(x, y, 0.0f, 0.0f);
      Gdiplus::StringFormat f;
      // f.SetAlignment(Gdiplus::StringAlignmentCenter);

      switch (context.textBaseline.getType()) {
      case TextBaseline::TOP: break;
      case TextBaseline::HANGING: break;
      case TextBaseline::MIDDLE: f.SetLineAlignment(Gdiplus::StringAlignmentCenter); break;
      }

      g->DrawString(text2.data(), text2.size(), &font, rect, &f, &brush);
    }

    void drawImage(Surface & _img, double x, double y, double w, double h) {
      GDIPlusSurface & img = dynamic_cast<GDIPlusSurface&>(_img);
      g->DrawImage(&(*(img.bitmap)), Gdiplus::REAL(x), Gdiplus::REAL(y));
    }

  protected:
    std::shared_ptr<Gdiplus::Bitmap> bitmap;
    std::shared_ptr<Gdiplus::Graphics> g;
    Gdiplus::BitmapData data;     
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

    void save() {
      save_stack.push_back(default_surface.g->Save());
    }
    void restore() {
      assert(!save_stack.empty());
      default_surface.g->Restore(save_stack.back());
      save_stack.pop_back();
    }

    GDIPlusSurface & getDefaultSurface() { return default_surface; }
    const GDIPlusSurface & getDefaultSurface() const { return default_surface; }
    
    void resize(unsigned int _width, unsigned int _height) {
      Context::resize(_width, _height);
    }
    void flush() {
    }
    void beginPath() {
      current_path.Reset();  
    }
    void closePath() {
      current_path.CloseFigure();
    }
    void clip() {
#if 0
      default_surface.g->SetClip(&current_path, Gdiplus::CombineModeReplace);
#else
      Gdiplus::Region region(&current_path);
      default_surface.g->SetClip(&region);
#endif
      current_path.Reset();
    }
    void arc(double x, double y, double r, double a0, double a1, bool t = false);
    void clearRect(double x, double y, double w, double h) { }
    void moveTo(double x, double y) {
      current_position = Gdiplus::PointF(x, y);
    }
    void lineTo(double x, double y) {
      Gdiplus::PointF point(x, y);
      current_path.AddLine(current_position, point);
      current_position = point;
    }
    void stroke() {
      Gdiplus::Pen pen(Gdiplus::Color(strokeStyle.color.red, strokeStyle.color.green, strokeStyle.color.blue ));
      default_surface.g->DrawPath(&pen, &current_path);
    }
    void fill();
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
      return {(double)size.Width, (double)size.Height};
    }
    
  protected:
    Point getCurrentPoint() { return Point(current_position.X, current_position.Y); }

    GDIPlusSurface default_surface;
    Gdiplus::GraphicsPath current_path;
    Gdiplus::PointF current_position;
    std::vector<Gdiplus::GraphicsState> save_stack;

  private:   
    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;
  };

  class GDIPlusContextFactory : public ContextFactory  {
  public:
    GDIPlusContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) { return std::shared_ptr<Context>(new ContextGDIPlus(width, height)); }
  };
};
