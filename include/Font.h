#ifndef _FONT_H_
#define _FONT_H_

#include "Attribute.h"

namespace canvas {
  class Font : public Attribute {
  public:
    enum Slant {
      NORMAL_SLANT = 1,
      ITALIC,
      OBLIQUE
    };
    enum Weight {
      NORMAL = 1,
      BOLD,
      BOLDER,
      LIGHTER
#if 0
100
200
300
400
500
600
700
800
900
#endif
    };
    enum TextDecoration {
      NO_DECORATION,
      UNDERLINE
    };
    Font() { }
    Font(const std::string & s) { }

    std::string family = "sans-serif";
    float size = 10.0f;
    Slant slant = NORMAL_SLANT;
    Weight weight = NORMAL;
    TextDecoration decoration = NO_DECORATION;
    bool antialiasing = true;
    bool hinting = true;
    bool cleartype = false;
  };
};

#endif
