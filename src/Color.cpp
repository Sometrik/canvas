#include "Color.h"

using namespace canvas;

static int get_xdigit(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else {
    return 0;
  }
}

Color::Color(const std::string & s) {
  red = green = blue = 0;
  setValue(s);
}

Color &
Color::operator=(const std::string & s) {
  setValue(s);
  return *this;
}

void
Color::setValue(const std::string & s) {
  if (s == "black") {
    red = green = blue = 0;
  } else if (s == "white") {
    red = green = blue = 255;
  } else {
    unsigned int pos = 0;
    if (s.size() && s[0] == '#') pos++;
    if (s.size() >= pos + 6) {
      red = get_xdigit(s[pos]) * 16 + get_xdigit(s[pos+1]);
      green = get_xdigit(s[pos+2]) * 16 + get_xdigit(s[pos+3]);
      blue = get_xdigit(s[pos+4]) * 16 + get_xdigit(s[pos+5]);
    } else if (s.size() >= pos + 3) {
      red = get_xdigit(s[pos]);
      green = get_xdigit(s[pos+1]);
      blue = get_xdigit(s[pos+2]);
      red = red * 16 + red;
      green = green * 16 + green;
      blue = blue * 16 + blue;
    } else {
      red = green = blue = 0;
    }
  }
}
