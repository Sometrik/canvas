#ifndef _STYLE_H_
#define _STYLE_H_

#include <string>

#include "Color.h"

namespace canvas {
  class Style {
  public:
    Style() { }
    Style(const std::string & s);
    Style & operator=(const std::string & s);
    
    Color color;
  };
};

#endif
