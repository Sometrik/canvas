#ifndef _COLOR_H_
#define _COLOR_H_

#include <string>

namespace canvas {
  class Color {
  public:
  Color() : red(0.0f), green(0.0f), blue(0.0f), alpha(1.0f) { }
    Color(const std::string & s)
      : red(0.0f), green(0.0f), blue(0.0f), alpha(1.0f) {
      setValue(s);
    }
  Color(float _red, float _green, float _blue, float _alpha)
    : red(_red), green(_green), blue(_blue), alpha(_alpha) { }
    
    Color & operator=(const std::string & s);
    
    float red, green, blue, alpha;
    
  private:
    void setValue(const std::string & s);
  };
};

#endif
