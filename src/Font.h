#ifndef _FONT_H_
#define _FONT_H_

#include <string>

namespace canvas {
  class Font {
  public:
    Font() { }
    Font(const std::string & s);

    std::string family;
    float size = 10.0f;
  };
};

#endif
