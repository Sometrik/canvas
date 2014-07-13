#include "Context.h"

using namespace canvas {

  class ContextWx : public Context {
  ContextWx(wxGraphicsContext * _gc) :
    gc(_gc),
      current_path(_gc->CreatePath())
	{
    
	}
    ~ContextWx() {
      delete gc;
    }
    
    void clearRect(float x, float y, float w, float h) {
      
    }
    void fillRect(float x, float y, float w, float h) {
      wxGraphicsPath path = gc->CreatePath();
      path.AddRectangle(x, y, w, h);
      gc->FillPath(path);
    }
    void moveTo(float x, float y) {
      current_path.MoveToPoint(x, y);
    }
    void lineTo(float x, float y) {
      current_path.AddLineToPoint(x, y);
    }
    void stroke() {
      gc->StrokePath(current_path);
      current_path = gc->CreatePath();    
    }
    void fill() {
      gc->FillPath(current_path);
      current_path = gc->CreatePath();    
    }

  private:
    wxGraphicsContext * gc;
    wxGraphicsPath current_path;
    // gc->SetPen( *wxRED_PEN );
    // path.AddCircle( 50.0, 50.0, 50.0 );
    // path.CloseSubpath();
    // gc->StrokePath(path);
  };
};
