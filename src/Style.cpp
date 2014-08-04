#include "Style.h"

using namespace canvas;

Style::Style(const std::string & s) : color(s) {
 
}

Style &
Style::operator=(const std::string & s) {
  color = s;
  type = SOLID;
  return *this;
}
