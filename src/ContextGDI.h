#ifndef _GDICONTEXT_H_
#define _GDICONTEXT_H_

#include <Windows.h>
#include <cassert>

#include "Context.h"

//Gradient
#pragma comment( lib, "msimg32.lib" )
///

namespace canvas {
  inline COLORREF ColorToColorRef(const Color& color) {
    return RGB(color.red, color.green, color.blue);
  }

  class GDISurface : public Surface {
  public:
    friend class ContextGDI;

    GDISurface(unsigned int _width, unsigned int _height)
      : Surface(_width, _height),
      final_data(0)
    {
      hdc = ::CreateCompatibleDC(0);
      assert(hdc != NULL);
      hbmp = ::CreateCompatibleBitmap(hdc, _width, _height);
      assert(hbmp != NULL);
      select();
    }
    GDISurface(unsigned int _width, unsigned int _height, unsigned char * data)
      : Surface(_width, _height),
      final_data(0)
    {
      // const SIZE& size, void** pBits)
#if 0
    assert(0 < size.cx);
    assert(0 != size.cy);

    if (pBits != NULL)
    {
        *pBits = NULL;
    }

    if (size.cx <= 0 || size.cy == 0)
    {
        return NULL;
    }

    BITMAPINFO bi = {0};
    // Fill in the BITMAPINFOHEADER
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = size.cx;
    bi.bmiHeader.biHeight      = size.cy;
    bi.bmiHeader.biSizeImage   = size.cx * abs(size.cy);
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    LPVOID pData = NULL;
    HBITMAP hbmp = ::CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pData, NULL, 0);

    if (pData != NULL && hbmp != NULL && pBits != NULL)
    {
        *pBits = pData;
    }
#endif
    }
    ~GDISurface() {
      ::DeleteDC(hdc);
      ::DeleteObject(hbmp);
      // delete[] final_data;
    }

    void select() {
      SelectObject(hdc, hbmp);
    }

    void flush() {
      delete[] final_data;
      BITMAP bmp;
      GetObject(hbmp, sizeof(BITMAP), &bmp);
#if 0
      unsigned int bmp_size = bmp.bmWidthBytes * bmp.bmHeight;
      final_data = new unsigned char[bmp_size];
      memset(final_data, 0, bmp_size);
      GetBitmapBits(hbmp, bmp_size, final_data);
#else
      final_data = (unsigned char*)bmp.bmBits;
#endif
    }

    unsigned char * getBuffer() { return final_data; }
    const unsigned char * getBuffer() const { return final_data; }
    
  protected:
    HDC hdc;
    HBITMAP hbmp;
    unsigned char * final_data;
  };

  class ContextGDI : public Context {
  public:
    ContextGDI(unsigned int _width, unsigned int _height)
      : Context(_width, _height), default_surface(_width, _height)
    {
      beginPath();
    }
    ~ContextGDI() { }

    void beginPath() {
        BeginPath(default_surface.hdc);
    }
    void closePath() {
        CloseFigure(default_surface.hdc);
        EndPath(default_surface.hdc);
    }
    void clip() {
        // The clip() method must create a new clipping region by calculating the intersection
        //of the current clipping region and the area described by the current path, using the
        //non-zero winding number rule. Open subpaths must be implicitly closed when computing
        //the clipping region, without affecting the actual subpaths. The new clipping region
        //replaces the current clipping region.
        //When the context is initialized, the clipping region must be set to the rectangle with the top left corner at (0,0) and the width and height of the coordinate space.
        EndPath(default_surface.hdc);
        SelectClipPath(default_surface.hdc, RGN_AND);
        // FIX
    }
    void save() { }
    void restore() { }
    void arc(double x, double y, double r, double a0, double a1, bool t = false) { }
    void clearRect(double x, double y, double w, double h) { }

    void moveTo(double x, double y) {
      ::MoveToEx(default_surface.hdc, int(x), int(y), NULL);
    }

    void lineTo(double x, double y) {
      ::LineTo(default_surface.hdc, int(x), int(y));
    }

    void stroke() {
      closePath();

      COLORREF color = ColorToColorRef(strokeStyle.color);
      HPEN hpen = CreatePen(PS_SOLID, (int)lineWidth, color); // FIX THIS
      HPEN oldPen = (HPEN) SelectObject(default_surface.hdc, hpen);
      StrokePath(default_surface.hdc);
      SelectObject(default_surface.hdc, oldPen);
      DeleteObject(hpen);
    }

    void fill() {
      closePath();

      COLORREF color = ColorToColorRef(fillStyle.color);
      LOGBRUSH logbrush;
      logbrush.lbColor = color;
      logbrush.lbStyle = BS_SOLID;
      logbrush.lbHatch = 0;
      HBRUSH hBrush = CreateBrushIndirect(&logbrush);
      HBRUSH oldBrush = (HBRUSH)SelectObject(default_surface.hdc, hBrush);
      FillPath(default_surface.hdc);
      SelectObject(default_surface.hdc, oldBrush);
      DeleteObject(hBrush);
    }   
        
#if 0
    void GdiTextOut(TextAlign textAlign, TextBaseline baseLine, const wchar_t* psz, int textlen, int ix, int iy) {
        assert(GetTextAlign(m_hDC) == TA_LEFT);
        SIZE sz;
        ::GetTextExtentPointW(m_hDC, psz, textlen, &sz);
        int newx = ix;
        int newy = iy;

        switch (textAlign)
        {
        case TextAlignStart:
            newx = ix;
            break;

        case TextAlignEnd:
            newx = ix - sz.cx;
            break;

        case TextAlignLeft:
            newx = ix;
            break;

        case TextAlignRight:
            newx = ix - sz.cx;
            break;

        case TextAlignCenter:
            newx = ix - sz.cx / 2;
            break;

        default:
            assert(false);
        }

        switch (baseLine)
        {
        case TextBaselineTop :
            newy = iy;
            break;//The top of the em square

        case TextBaselineHanging:
            newy = iy;
            break; //The hanging baseline

        case TextBaselineMiddle :
            newy = iy - sz.cy / 2;
            break; //The middle of the em square

        case TextBaselineAlphabetic :
        {
            TEXTMETRIC tm;
            GetTextMetrics(m_hDC, &tm);
            newy = iy - (tm.tmHeight - tm.tmDescent);
        }
        break;//The alphabetic baseline

        case TextBaselineIdeographic :
        {
            TEXTMETRIC tm;
            GetTextMetrics(m_hDC, &tm);
            newy = iy - (tm.tmHeight);
        }
        break; //The ideographic baseline

        case TextBaselineBottom :
            newy = iy - sz.cy;
            break;//The bottom of the em square

        default:
            break;
        }

        TextOut(m_hDC, newx, newy, psz, wcslen(psz));
    }
#endif

    void fillText(const std::string & text, double x, double y) {
      std::wstring text2;
      for (unsigned int i = 0; i < text.size(); i++) {
	text2 += (wchar_t)text[i];
      }
      const int ix = (int)x;
      const int iy = (int)y;

      COLORREF color = ColorToColorRef(fillStyle.color);
      COLORREF oldcolor = SetTextColor(default_surface.hdc, color);

      int oldbkmode = SetBkMode(default_surface.hdc, TRANSPARENT);
#if 0
      HFONT oldfont = (HFONT) SelectObject(default_surface.hdc, (HFONT)font.m_pNativeObject);
#endif
#if 0
      GdiTextOut(default_surface.hdc, textAlign, textBaseline, psz, wcslen(psz), ix, iy);
#else
      TextOut(default_surface.hdc, ix, iy, text2.data(), text2.size());
#endif
      SetTextColor(default_surface.hdc, oldcolor);
      SetBkMode(default_surface.hdc, oldbkmode);
#if 0
      SelectObject(default_surface.hdc, oldfont);
#endif
    }

    Size measureText(const std::string & text) {
      std::wstring text2;
      for (unsigned int i = 0; i < text.size(); i++) {
	text2 += (wchar_t)text[i];
      }
#if 0
      SelectObject(default_surface.hdc, (HFONT)font.m_pNativeObject);
#endif
      SIZE sz;
      ::GetTextExtentPoint(default_surface.hdc, text2.data(), text2.size(), &sz);
      return { sz.cx, sz.cy };
    }
    
    Surface & getDefaultSurface() { return default_surface; }
    const Surface & getDefaultSurface() const { return default_surface; }

    void drawImage(Surface & input, double x, double y, double w, double h) {
      GDISurface & img = dynamic_cast<GDISurface&>(input);
      BITMAP bm;
      GetObject(img.hbmp, sizeof(BITMAP), &bm);
      BOOL f = BitBlt(default_surface.hdc, x, y, bm.bmWidth, bm.bmHeight, img.hdc, 0, 0, SRCCOPY);
    }

  private:
    GDISurface default_surface;
    // int flags;
    // std::vector<CanvasStateInfo*> m_stack;
  };
};

#endif
