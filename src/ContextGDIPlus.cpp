#include "ContextGDIPlus.h"

bool canvas::ContextGDIPlus::is_initialized = false;
ULONG_PTR canvas::ContextGDIPlus::m_gdiplusToken;

using namespace std;
using namespace canvas;

void
ContextGDIPlus::fill() {
  if (fillStyle.getType() == Style::LINEAR_GRADIENT) {
    const std::map<float, Color> & colors = fillStyle.getColors();
    if (!colors.empty()) {
      std::map<float, Color>::const_iterator it0 = colors.begin(), it1 = colors.end();
      it1--;
      const Color & c0 = it0->second, c1 = it1->second;
      Gdiplus::LinearGradientBrush brush(Gdiplus::RectF(fillStyle.x0, fillStyle.y0, fillStyle.x1 - fillStyle.x0, fillStyle.y1 - fillStyle.y0), Gdiplus::Color(c0.red, c0.green, c0.blue), Gdiplus::Color(c1.red, c1.green, c1.blue), Gdiplus::LinearGradientModeHorizontal);
      default_surface.g->FillPath(&brush, &current_path);
    }
  } else {
    Gdiplus::SolidBrush brush(Gdiplus::Color(fillStyle.color.red, fillStyle.color.green, fillStyle.color.blue));
    default_surface.g->FillPath(&brush, &current_path);
  }
}

void
ContextGDIPlus::arc(double x, double y, double r, double sa, double ea, bool anticlockwise) {
  double span = 0;
  if ((!anticlockwise && (ea - sa >= 2 * M_PI)) || (anticlockwise && (sa - ea >= 2 * M_PI))) {
    // If the anticlockwise argument is false and endAngle-startAngle is equal to or greater than 2*PI, or, if the
    // anticlockwise argument is true and startAngle-endAngle is equal to or greater than 2*PI, then the arc is the whole
    // circumference of this circle.
    span = 2 * M_PI;
  } else {
    if (!anticlockwise && (ea < sa)) {
      span += 2 * M_PI;
    } else if (anticlockwise && (sa < ea)) {
      span -= 2 * M_PI;
    }
 
#if 0
    // this is also due to switched coordinate system
    // we would end up with a 0 span instead of 360
    if (!(qFuzzyCompare(span + (ea - sa) + 1, 1.0) && qFuzzyCompare(qAbs(span), 360.0))) {
      // mod 360
      span += (ea - sa) - (static_cast<int>((ea - sa) / 360)) * 360;
    }
#else
    span += ea - sa;
#endif
  }
 
#if 0
  // If the path is empty, move to where the arc will start to avoid painting a line from (0,0)
  // NOTE: QPainterPath::isEmpty() won't work here since it ignores a lone MoveToElement
  if (!m_path.elementCount())
    m_path.arcMoveTo(xs, ys, width, height, sa);
  else if (!radius) {
    m_path.lineTo(xc, yc);
    return;
  }
#endif

  Gdiplus::RectF rect(Gdiplus::REAL(x - r), Gdiplus::REAL(y - r), Gdiplus::REAL(2 * r), Gdiplus::REAL(2 * r));
  current_path.AddArc(rect, Gdiplus::REAL(sa * 180 / M_PI), Gdiplus::REAL(span * 180 / M_PI));
}


