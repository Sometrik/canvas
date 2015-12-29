#include <Style.h>

#include <GraphicsState.h>

using namespace canvas;

#if 0
Style::Style(const std::string & s) : color(s) { }
Style::Style(const Color & _color) : color(_color) { }
#endif

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

GraphicsState &
Style::operator()(const std::string & s) {
  color = s;
  type = SOLID;
  return *context;
}
					
GraphicsState &
Style::operator()(const Color & _color) {
  color = _color;
  type = SOLID;
  return *context;
}
					
