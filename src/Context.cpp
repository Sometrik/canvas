#include "Context.h"

#include <cmath>
#include <iostream>

#ifdef OPENGL
#include <glm/glm.hpp>
#endif

using namespace std;
using namespace canvas;

void
Context::resize(unsigned int _width, unsigned int _height) {
  width = _width;
  height = _height;
  getDefaultSurface().resize(_width, _height);
}

void
Context::rect(double x, double y, double w, double h) {
  moveTo(x, y);
  lineTo(x + w, y);
  lineTo(x + w, y + h);
  lineTo(x, y + h); 
  closePath();
}

void
Context::fillRect(double x, double y, double w, double h) {
  beginPath();
  rect(x, y, w, h);  
  fill();
} 

void
Context::strokeRect(double x, double y, double w, double h) {
  beginPath();
  rect(x, y, w, h);  
  stroke();
}

void
Context::fillText(const std::string & text, double x, double y) {  
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    auto shadow = createSurface(getDefaultSurface().getWidth() + 2 * int(ceil(bx)), getDefaultSurface().getHeight() + 2 * int(ceil(by)));
    Style shadow_style = shadowColor;
    float shadow_alpha = shadowColor.alpha;
    shadow_style.color.alpha = 1.0f;
    shadow->fillText(font, shadow_style, textBaseline, textAlign, text, x + shadowOffsetX + bx, y + shadowOffsetY + by);
    shadow->gaussianBlur(bx, by, shadow_alpha);
    getDefaultSurface().drawImage(*shadow, -bx, -by, shadow->getWidth(), shadow->getHeight());
  }
  getDefaultSurface().fillText(font, fillStyle, textBaseline, textAlign, text, x, y);
}

void
Context::strokeText(const std::string & text, double x, double y) {  
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    auto shadow = createSurface(getDefaultSurface().getWidth() + 2 * int(ceil(bx)), getDefaultSurface().getHeight() + 2 * int(ceil(by)));
    Style shadow_style = shadowColor;
    shadow->strokeText(font, shadow_style, textBaseline, textAlign, text, x + shadowOffsetX + bx, y + shadowOffsetY + by);
    shadow->gaussianBlur(bx, by);
    getDefaultSurface().drawImage(*shadow, -bx, -by, shadow->getWidth(), shadow->getHeight());
  }
  getDefaultSurface().strokeText(font, strokeStyle, textBaseline, textAlign, text, x, y);
}

void
Context::fill() {
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    auto shadow = createSurface(getDefaultSurface().getWidth() + 2 * int(ceil(bx)), getDefaultSurface().getHeight() + 2 * int(ceil(by)));
    Style shadow_style = shadowColor;
    Path tmp_path = current_path;
    tmp_path.offset(shadowOffsetX + bx, shadowOffsetY + by);
    
    shadow->fill(tmp_path, shadow_style);
    shadow->gaussianBlur(bx, by);
    
    getDefaultSurface().drawImage(*shadow, -bx, -by, shadow->getWidth(), shadow->getHeight());
  }
  getDefaultSurface().fill(current_path, fillStyle);
}

void
Context::stroke() {
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    auto shadow = createSurface(getDefaultSurface().getWidth() + 2 * int(ceil(bx)), getDefaultSurface().getHeight() + 2 * int(ceil(by)));
    Style shadow_style = shadowColor;
    Path tmp_path = current_path;
    tmp_path.offset(shadowOffsetX + bx, shadowOffsetY + by);
    
    shadow->stroke(tmp_path, shadow_style, lineWidth);
    shadow->gaussianBlur(bx, by);
    
    getDefaultSurface().drawImage(*shadow, -bx, -by, shadow->getWidth(), shadow->getHeight());    
  }
  getDefaultSurface().stroke(current_path, strokeStyle, lineWidth);
}

void
Context::drawImage(Surface & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    auto shadow = createSurface(getDefaultSurface().getWidth() + 2 * int(ceil(bx)), getDefaultSurface().getHeight() + 2 * int(ceil(by)));
    
    shadow->drawImage(img, x + shadowOffsetX + bx, y + shadowOffsetY + by, w, h, globalAlpha, imageSmoothingEnabled);
    shadow->colorFill(shadowColor);
    shadow->gaussianBlur(bx, by);
    
    getDefaultSurface().drawImage(*shadow, -bx, -by, shadow->getWidth(), shadow->getHeight());    
  }
  getDefaultSurface().drawImage(img, x, y, w, h, globalAlpha, imageSmoothingEnabled);  
}

void
Context::save() {
  restore_stack.push_back(SavedContext());
  auto & data = restore_stack.back();
  data.globalAlpha = globalAlpha;
  data.current_path = current_path;
  getDefaultSurface().save();
}

void
Context::restore() {
  if (!restore_stack.empty()) {
    auto & data = restore_stack.back();
    globalAlpha = data.globalAlpha;
    current_path = data.current_path;
  }
  getDefaultSurface().restore();
}
