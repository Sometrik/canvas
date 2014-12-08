#ifndef _CANVAS_TEXTALIGN_H_
#define _CANVAS_TEXTALIGN_H_

namespace canvas {
  class TextAlign {
  public:
    enum TextAlignType {
      START = 1,
      END,
      LEFT,
      CENTER,
      RIGHT
    };
    
    TextAlign(TextAlignType _type = LEFT) : type(_type) { }
    TextAlign(const std::string & _type) { setType(_type.c_str()); }
    TextAlign(const char * _type) { setType(_type); }
    TextAlign & operator=(const std::string & _type) { setType(_type.c_str()); return *this; }
    TextAlign & operator=(const char * _type) { setType(_type); return *this; }

    TextAlignType getType() const { return type; }
    
  private:
    void setType(const char * _type) {
      if (strcmp(_type, "start") == 0) type = START;
      else if (strcmp(_type, "end") == 0) type = END;
      else if (strcmp(_type, "left") == 0) type = LEFT;
      else if (strcmp(_type, "center") == 0) type = CENTER;
      else if (strcmp(_type, "right") == 0) type = RIGHT;
      else {
	assert(0);
	type = START;
      }
    };

    TextAlignType type;
  };
};

#endif
