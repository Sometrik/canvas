#include "Context.h"

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gdiplus.h>

#include <iostream>
#include <cassert>

#include "utf8.h"

#pragma comment (lib, "gdiplus.lib")

namespace canvas {
  class GDIPlusSurface : public Surface {
  public:
    friend class ContextGDIPlus;

    GDIPlusSurface(unsigned int _width, unsigned int _height) : Surface(_width, _height), bitmap(_width, _height) {
    }
    // Gdiplus::PixelFormat32bppARGB
    GDIPlusSurface(unsigned int _width, unsigned int _height, unsigned char * _data) : Surface(_width, _height),
      bitmap(_width, _height, _width * 3, Gdiplus::PixelFormat24bppRGB, _data)
    {
    }
    ~GDIPlusSurface() {
      delete[] output_data;
    }
    void resize(unsigned int _width, unsigned int _height) {
      Surface::resize(_width, _height);
      bitmap = Gdiplus::Bitmap(_width, _height);
    }
    void flush() {
      Gdiplus::Rect rect(0, 0, bitmap.GetWidth(), bitmap.GetHeight());
      Gdiplus::BitmapData data;
      bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);
      // glPixelStorei(GL_UNPACK_ROW_LENGTH, data.Width);
      delete[] output_data;
      size_t s = bitmap.GetWidth() * bitmap.GetHeight() * 4;
      output_data = new unsigned char[s];
      memcpy(output_data, data.Scan0, s);
      bitmap.UnlockBits(&data);
    }

    unsigned char * getBuffer() {
      return output_data;
    }
    const unsigned char * getBuffer() const {
      return output_data;
    }

  protected:
    // HDC dc;
    // HBITMAP bitmap;
    Gdiplus::Bitmap bitmap;
    unsigned char * output_data = 0;
  };

  class ContextGDIPlus : public Context {
  public:
    ContextGDIPlus(unsigned int _width = 0, unsigned int _height = 0)
      : Context(_width, _height),
	default_surface(_width, _height),
	g(&default_surface.bitmap)
    {
      g.Clear(Gdiplus::Color::Green);
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

    void save() {
      save_stack.push_back(g.Save());
    }
    void restore() {
      assert(!save_stack.empty());
      g.Restore(save_stack.back());
      save_stack.pop_back();
    }

    GDIPlusSurface & getDefaultSurface() { return default_surface; }
    const GDIPlusSurface & getDefaultSurface() const { return default_surface; }
    
    void resize(unsigned int _width, unsigned int _height) {
      Context::resize(_width, _height);
      g = Gdiplus::Graphics(&default_surface.bitmap);
    }
    void flush() {
    }
    void beginPath() {
      current_path.Reset();  
    }
    void closePath() {
    }
    void clip() {
      g.SetClip(current_path, Gdiplus::CombineModeReplace);
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
      g.DrawPath(&pen, &current_path);
    }
    void fill() {
      Gdiplus::SolidBrush brush(Gdiplus::Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue ));
      g.FillPath(&brush, &current_path);
    }
    void fillText(const std::string & text, double x, double y) {
      std::wstring text2 = convert_to_string(text);
      Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), font.size);
      // LinearGradientBrush brush(Rect(0,0,100,100), Color::Red, Color::Yellow, LinearGradientModeHorizontal);
      Gdiplus::SolidBrush brush(Gdiplus::Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue ));
      g.DrawString(text2.data(), text2.size(), &font, Gdiplus::PointF(x, y), &brush);
    }
    Size measureText(const std::string & text) {
      std::wstring text2 = convert_to_string(text);
      Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), font.size);
      Gdiplus::RectF layoutRect(0, 0, 512, 512), boundingBox;
      MeasureString(text2.data(), text2.size(), &font, &layoutRect, &boundingBox);
      Gdiplus::SizeF size = boundingBox.GetSize();
      return Size(size.Width, size.height);
    }

    void drawImage(Context & other, double x, double y, double w, double h) {
      Context::drawImage(other, x, y, w, h);
    }
    void drawImage(Surface & _img, double x, double y, double w, double h) {
      GDIPlusSurface & img = dynamic_cast<GDIPlusSurface&>(_img);
      g.DrawImage(img.bitmap, x, y);
    }
    
  protected:
    static wstring convert_to_wstring(const std::string & input) {
      const char * str = input.c_str();
      const char * str_i = str;
      const char * end = str + input.size();
      wstring output;
      while ( str_i < end ) {
	output += (wchar_t)utf8::next(str_i, end);
      }
      return output;
    }

    GDIPlusSurface default_surface;
    Gdiplus::Graphics g;
    Gdiplus::GraphicsPath current_path;
    Gdiplus::PointF current_position;
    std::vector<Gdiplus::GraphicsState> save_stack;

  private:   
    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;
  };

  class GDIPlusContextFactory {
  public:
    GDIPlusContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) { return std::shared_ptr<Context>(new ContextGDIPlus(width, height)); }
  };
};
