#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

#include <string>

namespace canvas {
  class GraphicsState;
  
  class Attribute {
  public:
  Attribute(GraphicsState * _context) : context(_context) { }
    Attribute(const Attribute & other) = delete;
    virtual ~Attribute() { }

    Attribute & operator=(const Attribute & other) { return *this; }
    
  protected:
    GraphicsState * context;
  };
};

#endif
