#ifndef _STYLE_H_
#define _STYLE_H_

#include <string>
#include <map>

#include "Color.h"

namespace canvas {
  class Style {
  public:
    enum StyleType {
      SOLID = 1,
      LINEAR_GRADIENT
    };
    Style() { }
    Style(const std::string & s);
    Style(const Color & _color);
    Style & operator=(const std::string & s);
    Style & operator=(const Color & _color);

    StyleType getType() const { return type; }
    void setType(StyleType _type) { type = _type; }

    void addColorStop(float f, const Color & c) {
      colors[f] = c;
    }
    void addColorStop(float f, const std::string & s) {
      colors[f] = s;
    }
    void setVector(double _x0, double _y0, double _x1, double _y1) {
      x0 = _x0;
      y0 = _y0;
      x1 = _x1;
      y1 = _y1;
    }

    const std::map<float, Color> & getColors() const { return colors; }
    
    Color color;
    double x0 = 0, y0 = 0, x1 = 0, y1 = 0;

  private:
    StyleType type = SOLID;
    std::map<float, Color> colors;
  };
};

#endif
