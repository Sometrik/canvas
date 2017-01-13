#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

#include <string>

namespace canvas {
  class GraphicsState;
  
  class AttributeBase {
  public:
    AttributeBase(GraphicsState * _context) : context(_context) { }
    AttributeBase(const Attribute & other) = delete;
    virtual ~AttributeBase() { }

  protected:
    GraphicsState & getContext() { return *context; }
    
  private:
    GraphicsState * context;
  };

  template<class T>
  class Attribute : public AttributeBase {
  public:
    Attribute(GraphicsState * _context) : value() { }
    Attribute(GraphicsState * _context, T _value) : Attribute(_context), value(_value) { }
    Attribute(const Attribute<T> & other) : Attribute(other._context), value(other.value) { }
  
    Attribute<T> & operator=(const Attribute<T> & other) { value = other.value; return *this; }
    Attribute<T> & operator=(T _value) { value = _value; return *this; }

    GraphicsState & operator()(T _value) { value = _value; return getContext(); }

    T get() const { return value; }
    
  private:
    T value;
  };
};

#endif
