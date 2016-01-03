#ifndef _CANVAS_GRAPHICSSTATE_H_
#define _CANVAS_GRAPHICSSTATE_H_

#include <Style.h>
#include <Font.h>
#include <TextBaseline.h>
#include <TextAlign.h>
#include <Path.h>
#include <ColorAttribute.h>
#include <FloatAttribute.h>
#include <BoolAttribute.h>
#include <Matrix.h>

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
    GraphicsState(const GraphicsState & other)
      : lineWidth(this, other.lineWidth),
      fillStyle(this, other.fillStyle),
      strokeStyle(this, other.strokeStyle),
      shadowBlur(this, other.shadowBlur),
      shadowColor(this, other.shadowColor),
      shadowOffsetX(this, other.shadowOffsetX),
      shadowOffsetY(this, other.shadowOffsetY),    
      globalAlpha(this, other.globalAlpha),
      font(this, other.font),
      textBaseline(this, other.textBaseline),
      textAlign(this, other.textAlign),     
      imageSmoothingEnabled(this, other.imageSmoothingEnabled),
      currentPath(other.currentPath),
      clipPath(other.clipPath) {

    }
      
    GraphicsState & operator=(const GraphicsState & other) {
      if (this != &other) {
	lineWidth = other.lineWidth;
	fillStyle = other.fillStyle;
	strokeStyle = other.strokeStyle;
	shadowBlur = other.shadowBlur;
	shadowColor = other.shadowColor;
	shadowOffsetX = other.shadowOffsetX;
	shadowOffsetY = other.shadowOffsetY;
	globalAlpha = other.globalAlpha;
	font = other.font;
	textBaseline = other.textBaseline;
	textAlign = other.textAlign;
	imageSmoothingEnabled = other.imageSmoothingEnabled;
	currentPath = other.currentPath;
	clipPath = other.clipPath;	
      }
      return *this;
    }

    GraphicsState & arc(double x, double y, double r, double a0, double a1, bool t = false) { currentPath.arc(x, y, r, a0, a1, t); return *this; }
    GraphicsState & moveTo(double x, double y) { currentPath.moveTo(x, y); return *this; }
    GraphicsState & lineTo(double x, double y) { currentPath.lineTo(x, y); return *this; }
    GraphicsState & arcTo(double x1, double y1, double x2, double y2, double radius) { currentPath.arcTo(x1, y1, x2, y2, radius); return *this; }

    GraphicsState & clip() {
      clipPath = currentPath;
      currentPath.clear();
      return *this;
    }
    GraphicsState & resetClip() { clipPath.clear(); return *this; }
    GraphicsState & beginPath() { currentPath.clear(); return *this; }
    GraphicsState & closePath() { currentPath.closePath(); return *this; }
    GraphicsState & rect(double x, double y, double w, double h) {
      currentPath.rect(x, y, w, h);
      return *this;
    }

    void scale(double x, double y) {

    }
    void rotate(double angle) {

    }
    void translate(double x, double y) {

    }
    GraphicsState & transform(double a, double b, double c, double d, double e, double f) {
      currentTransform *= Matrix(a, b, c, d, e, f);
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
    const Matrix & getTransform() { return currentTransform; }
    
    FloatAttribute lineWidth;
    Style fillStyle;
    Style strokeStyle;
    FloatAttribute shadowBlur;
    ColorAttribute shadowColor;
    FloatAttribute shadowOffsetX, shadowOffsetY;
    FloatAttribute globalAlpha;
    Font font;
    TextBaselineAttribute textBaseline;
    TextAlignAttribute textAlign;
    BoolAttribute imageSmoothingEnabled;
    Path currentPath, clipPath;

  protected:
    Matrix currentTransform;
  };
};

#endif

