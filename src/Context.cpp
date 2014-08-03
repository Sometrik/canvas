#include "Context.h"

using namespace canvas;

void
Context::resize(unsigned int _width, unsigned int _height) {
  width = _width;
  height = _height;
  getDefaultSurface().resize(_width, _height);
}

void
Context::fillRect(double x, double y, double w, double h) {
  beginPath();
  moveTo(x, y);
  lineTo(x + w, y);
  lineTo(x + w, y + h);
  lineTo(x, y + h);
  closePath();
  fill();
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
