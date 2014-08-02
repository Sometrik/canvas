#include "Context.h"

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gdiplus.h>

#include <iostream>
#include <cassert>

#pragma comment (lib, "gdiplus.lib")

namespace canvas {
  class GDIPlusSurface : public Surface {
  public:
    friend class ContextGDIPlus;

    GDIPlusSurface(unsigned int _width, unsigned int _height) : Surface(_width, _height), bitmap(_width, _height) {
      if (!is_initialized) {
	is_initialized = true;
	// Initialize GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
      }
    }
    GDIPlusSurface(unsigned int _width, unsigned int _height, unsigned char * _data) : Surface(_width, _height),  bitmap(_width, _height) {

    }
    ~GDIPlusSurface() {
      delete[] output_data;
    }
    void resize(unsigned int width, unsigned int height) {
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

    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;
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

    void check() const {
    }
    void save() {
    }
    void restore() {
    }

    GDIPlusSurface & getDefaultSurface() { return default_surface; }
    const GDIPlusSurface & getDefaultSurface() const { return default_surface; }
    
    void resize(unsigned int width, unsigned int height) {
    }
    
    void flush() {
    }

    void beginPath() {
    }
    void closePath() {
    }
    void clip() {

    }
    void arc(double x, double y, double r, double a0, double a1, bool t = false) {
      current_path.AddArc(Gdiplus::REAL(x), Gdiplus::REAL(y), Gdiplus::REAL(2 * r), Gdiplus::REAL(2 * r), Gdiplus::REAL(a0), Gdiplus::REAL(a1 - a0));
    }
    void clearRect(double x, double y, double w, double h) { }
    void moveTo(double x, double y) {
      current_position = std::pair<double, double>(x, y);
    }
    void lineTo(double x, double y) {
      current_path.AddLine(Gdiplus::PointF(current_position.first, current_position.second), Gdiplus::PointF(x, y));
      current_position = std::pair<double, double>(x, y);
    }
    void stroke() {
      Gdiplus::Pen pen(Gdiplus::Color(strokeStyle.color.red, strokeStyle.color.green, strokeStyle.color.blue ));
      g.DrawPath(&pen, &current_path);
//      current_path.Clear();  
    }
    void fill() {
      Gdiplus::SolidBrush brush(Gdiplus::Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue ));
      g.FillPath(&brush, &current_path);
//      current_path.Clear();  
    }
    void fillText(const std::string & text, double x, double y) {
      std::wstring text2;
      for (unsigned int i = 0; i < text.size(); i++) {
	text2 += (wchar_t)text[i];
      }
      Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), 12);
      // LinearGradientBrush brush(Rect(0,0,100,100), Color::Red, Color::Yellow, LinearGradientModeHorizontal);
      Gdiplus::SolidBrush brush(Gdiplus::Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue ));
      g.DrawString(text2.data(), text2.size(), &font, Gdiplus::PointF(x, y), &brush);
    }
    Size measureText(const std::string & text) {
      return {0,0};
    }

    void drawImage(Context & other, double x, double y, double w, double h) {
      Context::drawImage(other, x, y, w, h);
    }
    void drawImage(Surface & img, double x, double y, double w, double h) {

    }

  protected:
    GDIPlusSurface default_surface;
    Gdiplus::Graphics g;
    Gdiplus::GraphicsPath current_path;
    std::pair<double, double> current_position;
  private:   
  };
};
