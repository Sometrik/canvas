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
  getDefaultSurface().resize(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()));
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
Context::renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y) {  
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    float bxs = bx * getDisplayScale(), bys = by * getDisplayScale();
    auto shadow = createSurface(getDefaultSurface().getActualWidth() + 2 * int(ceil(bxs)), getDefaultSurface().getActualHeight() + 2 * int(ceil(bys)));
    Style shadow_style = shadowColor;
    float shadow_alpha = shadowColor.alpha;
    shadow_style.color.alpha = 1.0f;
    shadow->renderText(mode, font, shadow_style, textBaseline, textAlign, text, x + shadowOffsetX + bx, y + shadowOffsetY + by, lineWidth, getDisplayScale());
    shadow->gaussianBlur(bxs, bys, shadow_alpha);
    getDefaultSurface().drawImage(*shadow, -bxs, -bys, shadow->getActualWidth(), shadow->getActualHeight());
  }
  getDefaultSurface().renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, getDisplayScale());
}

void
Context::renderPath(RenderMode mode, const Style & style) {
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    float bxs = bx * getDisplayScale(), bys = by * getDisplayScale();
    auto shadow = createSurface(getDefaultSurface().getActualWidth() + 2 * int(ceil(bxs)), getDefaultSurface().getActualHeight() + 2 * int(ceil(bys)));
    Style shadow_style = shadowColor;
    Path tmp_path = current_path;
    tmp_path.offset(shadowOffsetX + bx, shadowOffsetY + by);
    
    shadow->renderPath(mode, tmp_path, shadow_style, getDisplayScale() * lineWidth);
    shadow->gaussianBlur(bxs, bys);
    
    getDefaultSurface().drawImage(*shadow, -bxs, -bys, shadow->getActualWidth(), shadow->getActualHeight());
  }
  getDefaultSurface().renderPath(mode, current_path, style, getDisplayScale() * lineWidth);
}

void
Context::drawImage(Surface & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    float bx = shadowBlurX ? shadowBlurX : shadowBlur, by = shadowBlurY ? shadowBlurY : shadowBlur;
    float bxs = bx * getDisplayScale(), bys = by * getDisplayScale();
    auto shadow = createSurface(getDefaultSurface().getActualWidth() + 2 * int(ceil(bxs)), getDefaultSurface().getActualHeight() + 2 * int(ceil(bys)));
    
    shadow->drawImage(img, (x + bx + shadowOffsetX) * getDisplayScale(), (y + by + shadowOffsetY) * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
    shadow->colorFill(shadowColor);
    shadow->gaussianBlur(bx, by);
    
    getDefaultSurface().drawImage(*shadow, -bxs, -bys, shadow->getActualWidth(), shadow->getActualHeight());
  }
  getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
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
