#include "Context.h"

#include <wx/graphics.h>
#include <iostream>

namespace canvas {
  class WxSurface : public Surface {
  public:
    WxSurface(unsigned int _width, unsigned int _height) { }
    
    
  };
  
  class ContextWx : public Context {
  public:
    ContextWx(unsigned int _width = 0, unsigned int _height = 0)
      : Context(width, height),
      default_surface(_width, _height),
      gc(0) {
      resize(width, height);      
    }
    ~ContextWx() {
      // delete gc;
    }

    void resize(unsigned int width, unsigned int height) {
      std::cerr << "resizing to " << width << " " << height << std::endl;
      delete gc;
      if (width && height) {
	unsigned int pixelCount = width * height;
	unsigned char * rgbData = (unsigned char *)malloc(pixelCount * 3); // new unsigned char[pixelCount * 3];
	unsigned char * alphaData = (unsigned char *)malloc(pixelCount); // new unsigned char[pixelCount];
	memset(rgbData, 0, 3 * pixelCount);
	memset(alphaData, 0, pixelCount);
	std::cerr << "creating image\n";
	image = wxImage(wxSize(width, height), rgbData, alphaData);
	gc = wxGraphicsContext::Create(image);
      } else {
	gc = wxGraphicsContext::Create(); // dummy context for measuring	
      }
      current_path = gc->CreatePath();
      current_font = wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
      std::cerr << "done\n";
    }
    
    void flush() {
      std::cerr << "flushing\n";
      gc->Flush();
      // delete gc;
      // gc = 0;      
    }

    void beginPath() {
      // path.CloseSubpath();
    }
    void closePath() {
      // path.CloseSubpath();    
    }
    void clip() {
      wxRegion region;
      assert(0);
      gc->Clip(region);
    }
    void arc(double x, double y, double r, double a0, double a1, bool t) {

    }
    void clearRect(double x, double y, double w, double h) {
      
    }
    void fillRect(double x, double y, double w, double h) {
      wxGraphicsPath path = gc->CreatePath();
      path.AddRectangle(x, y, w, h);
      path.CloseSubpath();    
      fillPath(path);
    }
    void moveTo(double x, double y) {
      current_path.MoveToPoint(x, y);
    }
    void lineTo(double x, double y) {
      current_path.AddLineToPoint(x, y);
    }
    void stroke() {
      gc->SetPen(wxPen(wxColour(strokeStyle.color.red,
				strokeStyle.color.green,
				strokeStyle.color.blue)));
      gc->StrokePath(current_path);
      current_path = gc->CreatePath();    
    }
    void fill() {
      fillPath(current_path);
      current_path = gc->CreatePath();    
    }
    void fillText(const std::string & text, double x, double y) {
      std::cerr << "drawing text in color " << int(fillStyle.color.red) << " " << int(fillStyle.color.green) << " " << int(fillStyle.color.blue) << std::endl;
      assert(gc);
      gc->SetFont(current_font, wxColour(fillStyle.color.red,
					 fillStyle.color.green,
					 fillStyle.color.blue));
      gc->DrawText(wxString::FromUTF8(text.c_str()), x, y);
    }

    Size measureText(const std::string & text) {
      std::cerr << "trying to measure text " << text << std::endl;
      std::cerr << "setting fon\n";
      gc->SetFont(current_font, wxColour(fillStyle.color.red,
					 fillStyle.color.green,
					 fillStyle.color.blue));
      double width, height;
      std::cerr << "converting\n";
      wxString s = wxString::FromUTF8(text.c_str());
      std::cerr << "measuring\n";
      gc->GetTextExtent(s, &width, &height);
      std::cerr << "done\n";
      return { (float)width, (float)height };
    }    

  protected:
    void fillPath(wxGraphicsPath & path) {
      std::cerr << "filling path with color " << int(fillStyle.color.red)
		<< int(fillStyle.color.green)
		<< int(fillStyle.color.blue) << std::endl;
      gc->SetBrush(wxBrush(wxColour(fillStyle.color.red,
				    fillStyle.color.green,
				    fillStyle.color.blue)));
      gc->FillPath(path);
    }

    wxImage image;
    wxGraphicsContext * gc;
    
  private:
    wxGraphicsPath current_path;
    wxFont current_font;
    WxSurface default_surface;
  };
};