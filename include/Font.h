#ifndef _CANVAS_FONT_H_
#define _CANVAS_FONT_H_

#include <Attribute.h>
#include <FontWeight.h>

namespace canvas {
  class Font : public Attribute {
  public:
    enum Style {
      NORMAL_STYLE = 1,
      ITALIC,
      OBLIQUE
    };
    enum TextDecoration {
      NO_DECORATION,
      UNDERLINE
    };
    enum Variant {
      NORMAL_VARIANT,
      SMALL_CAPS
    };
  Font(GraphicsState * _context) : Attribute(_context) { }
  Font(GraphicsState * _context, const Font & other)
    : Attribute(_context),
      family(other.family),
      size(other.size),
      style(other.style),
      weight(other.weight),
      decoration(other.decoration),
      antialiasing(other.antialiasing),
      hinting(other.hinting),
      cleartype(other.cleartype) { }
      
#if 0
    Font(const std::string & s) { }
#endif
    
    std::string family = "sans-serif";
    float size = 10.0f;
    Style style = NORMAL_STYLE;
    FontWeight weight;
    TextDecoration decoration = NO_DECORATION;
    Variant variant = NORMAL_VARIANT;
    bool antialiasing = true;
    bool hinting = true;
    bool cleartype = false;
  };
};

#endif
