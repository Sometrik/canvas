#ifndef _CANVAS_GRAPHICSSTATE_H_
#define _CANVAS_GRAPHICSSTATE_H_

#include <Style.h>
#include <Font.h>
#include <TextBaseline.h>
#include <TextAlign.h>
#include <Path.h>
#include <ColorAttribute.h>

namespace canvas {
  class GraphicsState {
  public:
    GraphicsState()
      : fillStyle(this),
      strokeStyle(this),
      shadowColor(this),
      font(this),
      textBaseline(this),
      textAlign(this)
      { }
    GraphicsState(const GraphicsState & other)
      : lineWidth(other.lineWidth),
      fillStyle(this, other.fillStyle),
      strokeStyle(this, other.strokeStyle),
      shadowBlur(other.shadowBlur),
      shadowColor(this, other.shadowColor),
      shadowOffsetX(other.shadowOffsetX),
      shadowOffsetY(other.shadowOffsetY),    
      globalAlpha(other.globalAlpha),
      font(this, other.font),
      textBaseline(this, other.textBaseline),
      textAlign(this, other.textAlign),     
      imageSmoothingEnabled(other.imageSmoothingEnabled),
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

    void scale(double x, double y) {

    }
    void rotate(double angle) {

    }
    void translate(double x, double y) {

    }
    void transform(double a, double b, double c, double d, double e, double f) {

    }
    
    // DOMMatrix getTransform();
    // void setTransform(unrestricted double a, unrestricted double b, unrestricted double c, unrestricted double d, unrestricted double e, unrestricted double f);
    // void setTransform(optional DOMMatrixInit matrix);
    // void resetTransform();
    
    float lineWidth = 1.0f;
    Style fillStyle;
    Style strokeStyle;
    float shadowBlur = 0.0f;
    ColorAttribute shadowColor;
    float shadowOffsetX = 0.0f, shadowOffsetY = 0.0f;
    float globalAlpha = 1.0f;
    Font font;
    TextBaselineAttribute textBaseline;
    TextAlignAttribute textAlign;
    bool imageSmoothingEnabled = true;
    Path currentPath, clipPath;
  };
};

#endif

