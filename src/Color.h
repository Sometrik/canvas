#ifndef _COLOR_H_
#define _COLOR_H_

#include <string>

namespace canvas {
  class Color {
  public:
    Color() { }
    Color(const std::string & s);
    Color(unsigned char _red, unsigned char _green, unsigned char _blue)
      : red(_red), green(_green), blue(_blue) { }
    
    unsigned char red = 0, green = 0, blue = 0;
  };
};

#endif
