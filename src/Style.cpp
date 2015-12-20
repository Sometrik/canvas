#include <Style.h>

using namespace canvas;

Style::Style(const std::string & s) : color(s) { }
Style::Style(const Color & _color) : color(_color) { }

Style &
Style::operator=(const std::string & s) {
  color = s;
  type = SOLID;
  return *this;
}

Style &
Style::operator=(const Color & _color) {
  color = _color;
  type = SOLID;
  return *this;
}
