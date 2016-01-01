#ifndef _CANVAS_COLORATTRIBUTE_H_
#define _CANVAS_COLORATTRIBUTE_H_

#include "Attribute.h"
#include "Color.h"

#include <string>

namespace canvas {
  class ColorAttribute : public Attribute {
  public:
    ColorAttribute(GraphicsState * _context) : Attribute(_context) { }
    ColorAttribute(GraphicsState * _context, const ColorAttribute & other)
      : Attribute(_context),
      color(other.color)
	{ }
    ColorAttribute(const ColorAttribute & other) = delete;

    ColorAttribute & operator=(const ColorAttribute & other) { color = other.color; return *this; }
    ColorAttribute & operator=(const std::string & s) { color = s; return *this; }
    ColorAttribute & operator=(const Color & _color) { color = _color; return *this; }

    GraphicsState & operator()(const std::string & s) { color = s; return *context; }
    GraphicsState & operator()(const Color & _color) { color = _color; return *context; }

    const Color & getValue() const { return color; }
  private:
    Color color;
  };
};

#endif
