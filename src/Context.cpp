#include "Context.h"

#include <cmath>
#include <iostream>

using namespace std;
using namespace canvas;

void
Context::resize(unsigned int _width, unsigned int _height) {
  width = _width;
  height = _height;
  getDefaultSurface().resize(_width, _height);
}

void
Context::fillRect(double x, double y, double w, double h) {
  save();
  beginPath();
  moveTo(x, y);
  lineTo(x + w, y);
  lineTo(x + w, y + h);
  lineTo(x, y + h);
  closePath();
  fill();
  beginPath(); // tmp fix
  restore();
}

void
Context::strokeRect(double x, double y, double w, double h) {
  beginPath();
  moveTo(x, y);
  lineTo(x + w, y);
  lineTo(x + w, y + h);
  lineTo(x, y + h);
  closePath();
  stroke();
}

void
Context::fillText(const std::string & text, double x, double y) {  
  if (hasShadow()) {
    // cerr << "DRAWING SHADOW for text " << text << ", w = " << getWidth() << ", h = " << getHeight() << endl;
    auto shadow = createSurface(getDefaultSurface().getWidth(), getDefaultSurface().getHeight());
    Style tmp = fillStyle;
    fillStyle.color = shadowColor;
    shadow->fillText(*this, text, x + shadowOffsetX, y + shadowOffsetY);
    shadow->gaussianBlur(shadowBlur, shadowBlur);
    fillStyle = tmp;
    drawImage(*shadow, 0, 0, shadow->getWidth(), shadow->getHeight());
  }
  getDefaultSurface().fillText(*this, text, x, y);
}

// Implementation by node-canvas (Node canvas is a Cairo backed Canvas implementation for NodeJS)
// Original implementation influenced by WebKit.
void
Context::arcTo(double x1, double y1, double x2, double y2, double radius) {
  Point p0 = getCurrentPoint();
  Point p1(x1, y1);
  Point p2(x2, y2);
  
  if ((p1.x == p0.x && p1.y == p0.y) || (p1.x == p2.x && p1.y == p2.y) || radius == 0.f) {
    lineTo(p1.x, p1.y);
    // p2?
    return;
  }

  Point p1p0((p0.x - p1.x), (p0.y - p1.y));
  Point p1p2((p2.x - p1.x), (p2.y - p1.y));
  float p1p0_length = sqrt(p1p0.x * p1p0.x + p1p0.y * p1p0.y);
  float p1p2_length = sqrt(p1p2.x * p1p2.x + p1p2.y * p1p2.y);

  double cos_phi = (p1p0.x * p1p2.x + p1p0.y * p1p2.y) / (p1p0_length * p1p2_length);
  // all points on a line logic
  if (-1 == cos_phi) {
    lineTo(p1.x, p1.y);
    // p2?
    return;
  }

  if (1 == cos_phi) {
    // add infinite far away point
    unsigned int max_length = 65535;
    double factor_max = max_length / p1p0_length;
    Point ep((p0.x + factor_max * p1p0.x), (p0.y + factor_max * p1p0.y));
    lineTo(ep.x, ep.y);
    return;
  }

  double tangent = radius / tan(acos(cos_phi) / 2);
  double factor_p1p0 = tangent / p1p0_length;
  Point t_p1p0((p1.x + factor_p1p0 * p1p0.x), (p1.y + factor_p1p0 * p1p0.y));

  Point orth_p1p0(p1p0.y, -p1p0.x);
  double orth_p1p0_length = sqrt(orth_p1p0.x * orth_p1p0.x + orth_p1p0.y * orth_p1p0.y);
  double factor_ra = radius / orth_p1p0_length;

  double cos_alpha = (orth_p1p0.x * p1p2.x + orth_p1p0.y * p1p2.y) / (orth_p1p0_length * p1p2_length);
  if (cos_alpha < 0.f) {
    orth_p1p0 = Point(-orth_p1p0.x, -orth_p1p0.y);
  }

  Point p((t_p1p0.x + factor_ra * orth_p1p0.x), (t_p1p0.y + factor_ra * orth_p1p0.y));

  orth_p1p0 = Point(-orth_p1p0.x, -orth_p1p0.y);
  double sa = acos(orth_p1p0.x / orth_p1p0_length);
  if (orth_p1p0.y < 0.f) {
    sa = 2 * M_PI - sa;
  }

  bool anticlockwise = false;

  double factor_p1p2 = tangent / p1p2_length;
  Point t_p1p2((p1.x + factor_p1p2 * p1p2.x), (p1.y + factor_p1p2 * p1p2.y));
  Point orth_p1p2((t_p1p2.x - p.x),(t_p1p2.y - p.y));
  double orth_p1p2_length = sqrt(orth_p1p2.x * orth_p1p2.x + orth_p1p2.y * orth_p1p2.y);
  double ea = acos(orth_p1p2.x / orth_p1p2_length);

  if (orth_p1p2.y < 0) ea = 2 * M_PI - ea;
  if ((sa > ea) && ((sa - ea) < M_PI)) anticlockwise = true;
  if ((sa < ea) && ((ea - sa) > M_PI)) anticlockwise = true;

  lineTo(t_p1p0.x, t_p1p0.y);
  arc(p.x, p.y, radius, sa, ea, anticlockwise); // && M_PI * 2 != radius);  
}
