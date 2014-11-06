#include "ContextGDIPlus.h"

bool canvas::ContextGDIPlus::is_initialized = false;
ULONG_PTR canvas::ContextGDIPlus::m_gdiplusToken;

using namespace std;
using namespace canvas;

static Gdiplus::GraphicsPath toGDIPath(const Path & path) {
  Gdiplus::GraphicsPath output;
  output.StartFigure();
  Gdiplus::PointF current_pos;

  for (auto pc : path.getData()) {
    switch (pc.type) {
    case PathComponent::MOVE_TO:
      current_pos = Gdiplus::PointF(Gdiplus::REAL(pc.x0), Gdiplus::REAL(pc.y0));
      break;
    case PathComponent::LINE_TO:
      {
	Gdiplus::PointF point(pc.x0, pc.y0);
	output.AddLine(current_pos, point);
	current_pos = point;
      }
      break;
    case PathComponent::ARC:
      {
	Gdiplus::RectF rect(Gdiplus::REAL(pc.x - pc.radius), Gdiplus::REAL(pc.y - pc.radius), Gdiplus::REAL(2 * pc.radius), Gdiplus::REAL(2 * pc.radius));
	double span = pc.ea - pc.sa;
	output.AddArc(rect, Gdiplus::REAL(pc.sa * 180.0f / M_PI), Gdiplus::REAL(span * 180.0f / M_PI));
      }
      break;
    }
  }    
  return output;
}

static Gdiplus::Color toGDIColor(const Color & input) {
  int red = int(input.red * 255), green = int(input.green * 255), int(input.blue * 255), alpha = int(input.alpha * 255);
  if (red < 0) red = 0;
  else if (red > 255) red = 255;
  if (green < 0) green = 0;
  else if (green > 255) green = 255;
  if (blue < 0) blue = 0;
  else if (blue > 255) blue = 255;
  if (alpha < 0) alpha = 0;
  else if (alpha > 255) alpha = 255;
  return Gdiplus::Color(red, green, blue, alpha);
}

void
GDIPlusSurface::fill(const Path & input_path, const Style & style) {
  Gdiplus::GraphicsPath path = toGDIPath(input_path);
 
  if (style.getType() == Style::LINEAR_GRADIENT) {
    const std::map<float, Color> & colors = style.getColors();
    if (!colors.empty()) {
      std::map<float, Color>::const_iterator it0 = colors.begin(), it1 = colors.end();
      it1--;
      const Color & c0 = it0->second, c1 = it1->second;
      Gdiplus::LinearGradientBrush brush(Gdiplus::PointF(style.x0, style.y0),
					 Gdiplus::PointF(style.x1, style.y1),
					 toGDIPlusColor(c0),
					 toGDIPlusColor(c1));
      g->FillPath(&brush, &path);
    }
  } else {
    Gdiplus::SolidBrush brush(toGDIPlusColor(style.color));
    g->FillPath(&brush, &path);
  }
}

void
GDIPlusSurface::stroke(const Path & input_path, const Style & style, double lineWidth) {
  Gdiplus::GraphicsPath path = toGDIPath(input_path);
  Gdiplus::Pen pen(toGDIColor(style.color));
  g->DrawPath(&pen, &path);
}

void
GDIPlusSurface::clip(const Path & input_path) {
  Gdiplus::GraphicsPath path = toGDIPath(input_path);

  Gdiplus::Region region(&path);
  default_surface.g->SetClip(&region);
}

#if 0
void
ContextGDIPlus::arc(double x, double y, double r, double sa, double ea, bool anticlockwise) {
  double span = 0;
  if (0 && ((!anticlockwise && (ea - sa >= 2 * M_PI)) || (anticlockwise && (sa - ea >= 2 * M_PI)))) {
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

#if 0
  if (anticlockwise) {
    span = -M_PI / 2.0;
  } else {
    span = M_PI / 2.0;
  }
#endif

  Gdiplus::RectF rect(Gdiplus::REAL(x - r), Gdiplus::REAL(y - r), Gdiplus::REAL(2 * r), Gdiplus::REAL(2 * r));
  current_path.AddArc(rect, Gdiplus::REAL(sa * 180 / M_PI), Gdiplus::REAL(span * 180 / M_PI));
  current_path.GetLastPoint(&current_position);
}
#endif


