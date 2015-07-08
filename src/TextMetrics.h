#ifndef _TEXTMETRICS_H_
#define _TEXTMETRICS_H_

namespace canvas {
  class TextMetrics {
  public:
  TextMetrics() : width(0), height(0) { }
  TextMetrics(float _width, float _height) : width(_width), height(_height) { }
    
    float width, height;
  };
};

#endif
