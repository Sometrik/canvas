#ifndef _CANVAS_BOOLATTRIBUTE_H_
#define _CANVAS_BOOLATTRIBUTE_H_

#include "Attribute.h"

#include <string>

namespace canvas {
  class BoolAttribute : public Attribute {
  public:
    BoolAttribute(GraphicsState * _context, bool _value = false) : Attribute(_context), value(_value) { }
    BoolAttribute(GraphicsState * _context, const BoolAttribute & other)
      : Attribute(_context), value(other.value) { }
    BoolAttribute(const BoolAttribute & other) = delete;
    
    BoolAttribute & operator=(const BoolAttribute & other) { value = other.value; return *this; }
    BoolAttribute & operator=(const std::string & s) { value = s == "true" ? true : false; return *this; }
    BoolAttribute & operator=(bool _value) { value = _value; return *this; }

    GraphicsState & operator()(const std::string & s) { value = s == "true" ? true : false; return *context; }
    GraphicsState & operator()(bool _value) { value = _value; return *context; }

    bool getValue() const { return value; }
    
  private:
    bool value;
  };
};

#endif
