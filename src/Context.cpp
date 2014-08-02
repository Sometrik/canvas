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
  moveTo(x, y);
  lineTo(x + w, y);
  lineTo(x + w, y + h);
  lineTo(x, y + h);
  lineTo(x, y);
  fill();
}

void
Context::strokeRect(double x, double y, double w, double h) {
  moveTo(x, y);
  lineTo(x + w, y);
  lineTo(x + w, y + h);
  lineTo(x, y + h);
  lineTo(x, y);
  stroke();
}
