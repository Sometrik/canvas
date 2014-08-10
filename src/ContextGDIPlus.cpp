#include "ContextGDIPlus.h"

bool canvas::ContextGDIPlus::is_initialized = false;
ULONG_PTR canvas::ContextGDIPlus::m_gdiplusToken;

using namespace std;
using namespace canvas;

void
ContextGDIPlus::fill() {
  if (fillStyle.getType() == Style::LINEAR_GRADIENT && 0) {
#if 0
    const std::map<float, Color> & colors = fillStyle.getColors();
    if (!colors.empty()) {
      std::map<float, Color>::const_iterator it0 = colors.begin(), it1 = colors.end();
      it1--;
      const Color & c0 = it0->second, c1 = it1->second;
      Gdiplus::LinearGradientBrush brush(Gdiplus::PointF(fillStyle.x0, fillStyle.y0), Gdiplus::PointF(fillStyle.x1, fillStyle.y1), Gdiplus::Color(c0.red, c0.green, c0.blue), Gdiplus::Color(c1.red, c1.green, c1.blue), Gdiplus::LinearGradientModeHorizontal);
      default_surface.g->FillPath(&brush, &current_path);
    }
#endif
  } else {
    Gdiplus::SolidBrush brush(Gdiplus::Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue));
    default_surface.g->FillPath(&brush, &current_path);
  }
}


