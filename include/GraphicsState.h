#ifndef _GRAPHICSSTATE_H_
#define _GRAPHICSSTATE_H_

namespace canvas {
  class GraphicsState {
  public:
    GraphicsState() { }
    GraphicsState(const GraphicsState & other) :
    lineWidth(context.lineWidth),
      fillStyle(context.fillStyle),
      strokeStyle(context.strokeStyle),
      shadowBlur(context.shadowBlur),
      shadowColor(context.shadowColor),
      shadowOffsetX(context.shadowOffsetX),
      shadowOffsetY(context.shadowOffsetY),    
      globalAlpha(context.globalAlpha),
      font(context.font),
      textBaseline(context.textBaseline),
      textAlign(context.textAlign),     
      imageSmoothingEnabled(context.imageSmoothingEnabled),
      currentPath(context.currentPath),
      clipPath(context.clipPath) { }
      
    GraphicsState & operator=(const GraphicsState & other) {
      if (this != &other) {
	lineWidth = data.lineWidth;
	fillStyle = data.fillStyle;
	strokeStyle = data.strokeStyle;
	shadowBlur = data.shadowBlur;
	shadowColor = data.shadowColor;
	shadowOffsetX = data.shadowOffsetX;
	shadowOffsetY = data.shadowOffsetY;
	globalAlpha = data.globalAlpha;
	font = data.font;
	textBaseline = data.textBaseline;
	textAlign = data.textAlign;
	imageSmoothingEnabled = data.imageSmoothingEnabled;
	currentPath = data.currentPath;
	clipPath = data.clipPath;	
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
