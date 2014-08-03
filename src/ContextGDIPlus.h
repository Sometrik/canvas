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
      bitmap(new Gdiplus::Bitmap(_width, _height, PixelFormat32bppARGB)),
      g(new Gdiplus::Graphics(&(*bitmap)))
    {
    }
    // Gdiplus::PixelFormat32bppARGB
    GDIPlusSurface(unsigned int _width, unsigned int _height, unsigned char * _data) : Surface(_width, _height),
      bitmap(new Gdiplus::Bitmap(_width, _height, _width * 3, PixelFormat24bppRGB, _data)),
      g(new Gdiplus::Graphics(&(*bitmap)))
    {
    }
    ~GDIPlusSurface() {
//      delete[] output_data;
    }
    void resize(unsigned int _width, unsigned int _height) {
      Surface::resize(_width, _height);
      bitmap = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(_width, _height));
      g = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(&(*bitmap)));
    }
    void flush() {
      Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
      bitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data);
#if 0
      delete[] output_data;
      size_t s = bitmap->GetWidth() * bitmap->GetHeight() * 4;
      output_data = new unsigned char[s];
      memcpy(output_data, data.Scan0, s);
      bitmap->UnlockBits(&data);
#else
      output_data = (unsigned char*)data.Scan0;
#endif
    }
    void markDirty() {
      bitmap->UnlockBits(&data);
    }

    unsigned char * getBuffer() {
      return output_data;
    }
    const unsigned char * getBuffer() const {
      return output_data;
    }

    void fillText(Context & context, const std::string & text, double x, double y) {
      std::wstring text2 = convert_to_wstring(text);
      Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), context.font.size);
      // LinearGradientBrush brush(Rect(0,0,100,100), Color::Red, Color::Yellow, LinearGradientModeHorizontal);
      Gdiplus::SolidBrush brush(Gdiplus::Color(context.fillStyle.color.red, context.fillStyle.color.green, context.fillStyle.color.blue));
      g->DrawString(text2.data(), text2.size(), &font, Gdiplus::PointF(x, y), &brush);
    }

  protected:
    std::shared_ptr<Gdiplus::Bitmap> bitmap;
    std::shared_ptr<Gdiplus::Graphics> g;
    unsigned char * output_data = 0;
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

    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, unsigned char * data) {
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
    }
    void clip() {
#if 1
      default_surface.g->SetClip(&current_path, Gdiplus::CombineModeReplace);
#endif
      current_path.Reset();
    }
    void arc(double x, double y, double r, double a0, double a1, bool t = false) {
      current_path.AddArc(Gdiplus::REAL(x), Gdiplus::REAL(y), Gdiplus::REAL(2 * r), Gdiplus::REAL(2 * r), Gdiplus::REAL(a0), Gdiplus::REAL(a1 - a0));
    }
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
    void fill() {
      Gdiplus::SolidBrush brush(Gdiplus::Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue ));
      default_surface.g->FillPath(&brush, &current_path);
    }
    Size measureText(const std::string & text) {
      std::wstring text2 = convert_to_wstring(text);
      Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), font.size);
      Gdiplus::RectF layoutRect(0, 0, 512, 512), boundingBox;
      default_surface.g->MeasureString(text2.data(), text2.size(), &font, layoutRect, &boundingBox);
      Gdiplus::SizeF size;
      boundingBox.GetSize(&size);
      return {(double)size.Width, (double)size.Height};
    }

    void drawImage(Context & other, double x, double y, double w, double h) {
      Context::drawImage(other, x, y, w, h);
    }
    void drawImage(Surface & _img, double x, double y, double w, double h) {
      GDIPlusSurface & img = dynamic_cast<GDIPlusSurface&>(_img);
      default_surface.g->DrawImage(&(*(img.bitmap)), Gdiplus::REAL(x), Gdiplus::REAL(y));
    }
    
  protected:
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
