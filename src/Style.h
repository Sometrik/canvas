#ifndef _STYLE_H_
#define _STYLE_H_

#include <string>

namespace canvas {
  class Style {
  public:
    Style() { }
    Style(const std::string & s);
    
  private:
    unsigned char red = 0, green = 0, blue = 0;
  };
};

#endif
