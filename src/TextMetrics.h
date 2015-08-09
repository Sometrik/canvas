#ifndef _TEXTMETRICS_H_
#define _TEXTMETRICS_H_

namespace canvas {
  class TextMetrics {
  public:
  TextMetrics() : width(0) { }
  TextMetrics(float _width) : width(_width) { }
    
    float width;

#if 0
    float ideographicBaseline;
    float alphabeticBaseline;
    float hangingBaseline;
    float emHeightDescent;
    float emHeightAscent;
    float actualBoundingBoxDescent;
    float actualBoundingBoxAscent;
    float fontBoundingBoxDescent;
    float fontBoundingBoxAscent;
    float actualBoundingBoxRight;
    float actualBoundingBoxLeft;
#endif
  };
};

#endif
