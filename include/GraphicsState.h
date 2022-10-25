#ifndef _CANVAS_GRAPHICSSTATE_H_
#define _CANVAS_GRAPHICSSTATE_H_

#include <Style.h>
#include <Font.h>
#include <TextBaseline.h>
#include <TextAlign.h>
#include <Path2D.h>
#include <Matrix.h>

#include <cmath>

namespace canvas {
  class GraphicsState {
  public:
    GraphicsState()
      : lineWidth(this, 1.0f),
      fillStyle(this),
      strokeStyle(this),
      shadowBlur(this),
      shadowColor(this),
      shadowOffsetX(this), shadowOffsetY(this),
      globalAlpha(this, 1.0f),
      font(this),
      textBaseline(this),
      textAlign(this),
      imageSmoothingEnabled(this, true)
      { }

    GraphicsState & clip() {
      clipPath = currentPath;
      currentPath.clear();
      return *this;
    }
    GraphicsState & resetClip() { clipPath.clear(); return *this; }

    GraphicsState & scale(double x, double y) { return transform(Matrix(x, 0.0, 0.0, y, 0.0, 0.0)); }
    GraphicsState & rotate(double angle) {
      double cos_angle = cos(angle), sin_angle = sin(angle);
      return transform(Matrix(cos_angle, sin_angle, -sin_angle, cos_angle, 0.0, 0.0));
    }
    GraphicsState & translate(double x, double y) {
      return transform(Matrix(1.0, 0.0, 0.0, 1.0, x, y));
    }
    GraphicsState & transform(double a, double b, double c, double d, double e, double f) { return transform(Matrix(a, b, c, d, e, f)); }
    GraphicsState & transform(const Matrix & m) {
      currentTransform *= m;
      return *this;
    }
    GraphicsState & setTransform(double a, double b, double c, double d, double e, double f) {
      currentTransform = Matrix(a, b, c, d, e, f);
      return *this;
    }
    GraphicsState & setTransform(const Matrix & m) {
      currentTransform = m;
      return *this;
    }
    GraphicsState & resetTransform() {
      currentTransform = Matrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
      return *this;
    }
    const Matrix & getTransform() const { return currentTransform; }
    
    Attribute<float> lineWidth;
    Style fillStyle;
    Style strokeStyle;
    Attribute<float> shadowBlur;
    Attribute<Color> shadowColor;
    Attribute<float> shadowOffsetX, shadowOffsetY;
    Attribute<float> globalAlpha;
    Font font;
    TextBaselineAttribute textBaseline;
    TextAlignAttribute textAlign;
    Attribute<bool> imageSmoothingEnabled;
    Path2D currentPath, clipPath;
    Matrix currentTransform;

  protected:
    std::vector<float> lineDash;
  };
};

#endif

