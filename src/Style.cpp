#include "Style.h"

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

Style::Style(const std::string & s) {
  unsigned int pos = 0;
  if (str.size() && str[0] == '#') pos++;
  if (str.size() >= pos + 6) {
    red = get_xdigit(str[pos]) * 16 + get_xdigit(str[pos+1]);
    green = get_xdigit(str[pos+2]) * 16 + get_xdigit(str[pos+3]);
    blue = get_xdigit(str[pos+4]) * 16 + get_xdigit(str[pos+5]);
  } else if (str.size() >= pos + 3) {
    red = get_xdigit(str[pos]);
    green = get_xdigit(str[pos+2]);
    blue = get_xdigit(str[pos+4]);
    red = red * 16 + red;
    green = green * 16 + green;
    blue = blue * 16 + blue;
  } else {
    red = green = blue = 0;
  }
}
