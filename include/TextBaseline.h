#ifndef _CANVAS_TEXTBASELINE_H_
#define _CANVAS_TEXTBASELINE_H_

#include "Attribute.h"

#include <cstring>
#include <cassert>

namespace canvas {
  class TextBaseline : public Attribute {
  public:
    enum TextBaselineType {
      TOP = 1,
      HANGING,
      MIDDLE,
      ALPHABETIC,
      IDEOGRAPHIC,
      BOTTOM
    };
    TextBaseline(TextBaselineType _type = ALPHABETIC) : type(_type) { }
    TextBaseline(const std::string & _type) { setType(_type.c_str()); }
    TextBaseline(const char * _type) { setType(_type); }
    TextBaseline & operator=(const std::string & _type) { setType(_type.c_str()); return *this; }
    TextBaseline & operator=(const char * _type) { setType(_type); return *this; }

    TextBaselineType getType() const { return type; }

  private:
    void setType(const char * _type) {
      if (strcmp(_type, "top") == 0) type = TOP;
      else if (strcmp(_type, "hanging") == 0) type = HANGING;
      else if (strcmp(_type, "middle") == 0) type = MIDDLE;
      else if (strcmp(_type, "alphabetic") == 0) type = ALPHABETIC;
      else if (strcmp(_type, "ideographic") == 0) type = IDEOGRAPHIC;
      else if (strcmp(_type, "bottom") == 0) type = BOTTOM;
      else {
	assert(0);
	type = TOP;
      }
    };

    TextBaselineType type;
  };
};

#endif
