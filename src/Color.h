#ifndef _COLOR_H_
#define _COLOR_H_

#include <string>

namespace canvas {
  class Color {
  public:
    Color() { }
    Color(const std::string & s);
    
    unsigned char red = 0, green = 0, blue = 0;
  };
};

#endif
