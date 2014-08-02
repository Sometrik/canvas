#include "Context.h"

#include <gdiplus.h>
// #include <Windows.h>

#include <iostream>
#include <cassert>

#pragma comment (lib, "gdiplus.lib")

namespace canvas {
  class GDIPlusSurface : public Surface {
  public:
    friend class ContextGDIPlus;

    GDIPlusSurface(unsigned int _width, unsigned int _height) : Surface(_width, _height) {
      if (!is_initialized) {
	is_initialized = true;
	// Initialize GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
      }

      dc.CreateCompatibleDC(0);
      bitmap.CreateCompatibleBitmap(&dc, _width, _height);
      dc.SelectObject(bitmap);
      
      // dc.BitBlt(0, 0, 300, 300, &bufferDC, 0, 0, SRCCOPY);
    }
    GDIPlusSurface(unsigned int width, unsigned int height, unsigned char * data);
    ~CairoSurface();

    void resize(unsigned int width, unsigned int height);

    unsigned char * getBuffer();
    const unsigned char * getBuffer() const;

    // cairo_image_surface_get_stride(surface);

    cairo_surface_t * surface;

  protected:
    CDC dc;
    CBitmap bitmap;
  };

  class ContextGDIPlus : public Context {
  public:
    ContextCairo(unsigned int _width = 0, unsigned int _height = 0)
      : Context(_width, _height),
	default_surface(_width, _height),
	g(default_surface.dc)
    {
      g.Clear(GdiPlus::Color::Green);
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
      current_path.AddArc(x, y, 2 * r, 2 * r, a0, a1 - a0);
    }
    void clearRect(double x, double y, double w, double h) { }
    void moveTo(double x, double y) {
      current_position = pair<double, double>(x, y);
    }
    void lineTo(double x, double y) {
      current_path.AddLine(current_position->first, current_position->second, x, y);
      current_position = pair<double, double>(x, y);
    }
    void stroke() {
      GdiPlus::Pen pen(Color(strokeStyle.color.red, strokeStyle.color.green, strokeStyle.color.blue ));
      g.DrawPath(pen, current_path);
      current_path.Clear();  
    }
    void fill() {
      GdiPlus::SolidBrush brush(Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue ));
      g.FillPath(brush, current_path);
      current_path.Clear();  
    }
    void fillText(const std::string & text, double x, double y) {
      std::wstring text2;
      for (unsigned int i = 0; i < text.size(); i++) {
	text2 += (wchar_t)text[i];
      }
      GdiPlus::Font font(&FontFamily(L"Arial"), 12);
      // LinearGradientBrush brush(Rect(0,0,100,100), Color::Red, Color::Yellow, LinearGradientModeHorizontal);
      GdiPlus::Brush brush(Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue ));
      g.DrawString(text2.data(), text2.size(), &font, PointF(x, y), &brush);
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
    GdiPlus::Graphics g;
    GraphicsPath current_path;
    std::pair<double, double> current_position;

    static bool is_initialized;
    static ULONG_PTR m_gdiplusToken;    
  private:   
  };
};
