#ifndef _FONT_H_
#define _FONT_H_

#include <string>

namespace canvas {
  class Font {
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
    Font() { }
    Font(const std::string & s) { }

    std::string family = "sans-serif";
    float size = 10.0f;
    Slant slant = NORMAL_SLANT;
    Weight weight = NORMAL;
    bool antialiasing = true;
    bool hinting = true;
	bool cleartype = false;
  };
};

#endif
