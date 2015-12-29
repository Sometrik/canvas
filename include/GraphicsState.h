#ifndef _CANVAS_GRAPHICSSTATE_H_
#define _CANVAS_GRAPHICSSTATE_H_

#include <Style.h>
#include <Font.h>
#include <TextBaseline.h>
#include <TextAlign.h>
#include <Path.h>

namespace canvas {
  class GraphicsState {
  public:
    GraphicsState()
      : fillStyle(this),
      strokeStyle(this),
      font(this)
      { }
    GraphicsState(const GraphicsState & other)
      : lineWidth(other.lineWidth),
      fillStyle(this, other.fillStyle),
      strokeStyle(this, other.strokeStyle),
      shadowBlur(other.shadowBlur),
      shadowColor(other.shadowColor),
      shadowOffsetX(other.shadowOffsetX),
      shadowOffsetY(other.shadowOffsetY),    
      globalAlpha(other.globalAlpha),
      font(this, other.font),
      textBaseline(other.textBaseline),
      textAlign(other.textAlign),     
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
    
    float lineWidth = 1.0f;
    Style fillStyle;
    Style strokeStyle;
    float shadowBlur = 0.0f;
    Color shadowColor;
    float shadowOffsetX = 0.0f, shadowOffsetY = 0.0f;
    float globalAlpha = 1.0f;
    Font font;
    TextBaseline textBaseline;
    TextAlign textAlign;
    bool imageSmoothingEnabled = true;
    Path currentPath, clipPath;
  };
};

#endif

