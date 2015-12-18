#include "Context.h"

#include <cmath>
#include <iostream>

#ifdef OPENGL
#include <glm/glm.hpp>
#endif

using namespace std;
using namespace canvas;

GraphicsState::GraphicsState(const Context & context) :
  globalAlpha(context.globalAlpha),
  imageSmoothingEnabled(context.imageSmoothingEnabled),
  shadowBlur(context.shadowBlur),
  shadowColor(context.shadowColor),
  shadowOffsetX(context.shadowOffsetX),
  shadowOffsetY(context.shadowOffsetY),
  currentPath(context.currentPath),
  clipPath(context.clipPath),
  lineWidth(context.lineWidth),
  fillStyle(context.fillStyle),
  strokeStyle(context.strokeStyle),
  font(context.font),
  textAlign(context.textAlign),
  textBaseline(context.textBaseline)
{ }

void
Context::resize(unsigned int _width, unsigned int _height) {
  getDefaultSurface().resize(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), getDefaultSurface().getFormat());
  hit_regions.clear();
}

Context &
Context::fillRect(double x, double y, double w, double h) {
  return beginPath().rect(x, y, w, h).fill();
} 

Context &
Context::strokeRect(double x, double y, double w, double h) {
  return beginPath().rect(x, y, w, h).stroke();
}

Context &
Context::clearRect(double x, double y, double w, double h) {
  Path path;
  path.rect(x, y, w, h);
  Style style(Color(0.0f, 0.0f, 0.0f, 0.0f));
  renderPath(FILL, path, style, COPY);
  return *this;
}

Context &
Context::renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op) {
  if (hasNativeShadows()) {
    getDefaultSurface().renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, op, getDisplayScale(), globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath);
  } else {
    if (hasShadow()) {
      float b = shadowBlur, bs = shadowBlur * getDisplayScale();
      int bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      Style shadow_style = shadowColor;
      float shadow_alpha = shadowColor.alpha;
      shadow_style.color.alpha = 1.0f;
      shadow->renderText(mode, font, shadow_style, textBaseline, textAlign, text, x + shadowOffsetX + bi, y + shadowOffsetY + bi, lineWidth, op, getDisplayScale(), globalAlpha, 0.0f, 0.0f, 0.0f, shadowColor, clipPath);
#if 1
      shadow->slowBlur(bs, bs, shadow_alpha);
#else
      shadow->blur(bs);
#endif
      getDefaultSurface().drawImage(*shadow, -bi, -bi, shadow->getActualWidth(), shadow->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor, Path());
    }
    getDefaultSurface().renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, op, getDisplayScale(), globalAlpha, 0.0f, 0.0f, 0.0f, shadowColor, clipPath);
  }
  return *this;
}

Context &
Context::renderPath(RenderMode mode, const Path & path, const Style & style, Operator op) {
  if (hasNativeShadows()) {
    getDefaultSurface().renderPath(mode, path, style, lineWidth, op, getDisplayScale(), globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath);
  } else {
    if (hasShadow()) {
      float b = shadowBlur, bs = shadowBlur * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      Style shadow_style = shadowColor;
      Path tmp_path = path, tmp_clipPath = clipPath;
      tmp_path.offset(shadowOffsetX + bi, shadowOffsetY + bi);
      tmp_clipPath.offset(shadowOffsetX + bi, shadowOffsetY + bi);
      
      shadow->renderPath(mode, tmp_path, shadow_style, lineWidth, op, getDisplayScale(), globalAlpha, 0, 0, 0, shadowColor, tmp_clipPath);
#if 1
      shadow->slowBlur(bs, bs);
#else
      shadow->blur(bs);
#endif
      getDefaultSurface().drawImage(*shadow, -bi, -bi, shadow->getActualWidth(), shadow->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor, Path(), false);
    }
    getDefaultSurface().renderPath(mode, path, style, lineWidth, op, getDisplayScale(), globalAlpha, 0, 0, 0, shadowColor, clipPath);
  }
  return *this;
}

Context &
Context::drawImage(Surface & img, double x, double y, double w, double h) {
  if (hasNativeShadows()) {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath, imageSmoothingEnabled);
  } else {
    if (hasShadow()) {
      float b = shadowBlur, bs = shadowBlur * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
    
      shadow->drawImage(img, (x + bi + shadowOffsetX) * getDisplayScale(), (y + bi + shadowOffsetY) * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha, 0.0f, 0.0f, 0.0f, shadowColor, clipPath, imageSmoothingEnabled);
      // shadow->colorFill(shadowColor);
#if 1
      shadow->slowBlur(bs, bs);
#else
      shadow->blur(bs);
#endif
    
      getDefaultSurface().drawImage(*shadow, -bi, -bi, shadow->getActualWidth(), shadow->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor, Path());
    }
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha, 0.0f, 0.0f, 0.0f, shadowColor, clipPath, imageSmoothingEnabled);
  }
  return *this;
}

Context &
Context::drawImage(const Image & img, double x, double y, double w, double h) {
  if (hasNativeShadows()) {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath, imageSmoothingEnabled);
  } else {
    if (hasShadow()) {
      float b = shadowBlur, bs = shadowBlur * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
    
      shadow->drawImage(img, (x + bi + shadowOffsetX) * getDisplayScale(), (y + bi + shadowOffsetY) * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha, 0.0f, 0.0f, 0.0f, shadowColor, clipPath, imageSmoothingEnabled);
      // shadow->colorFill(shadowColor);
#if 1
      shadow->slowBlur(bs, bs);
#else
      shadow->blur(bs);
#endif
    
      getDefaultSurface().drawImage(*shadow, -bi, -bi, shadow->getActualWidth(), shadow->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor, Path(), false);
    }
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha, 0.0f, 0.0f, 0.0f, shadowColor, clipPath, imageSmoothingEnabled);
  }
  return *this;
}

Context &
Context::save() {
  restore_stack.push_back(GraphicsState(*this));
  return *this;
}

Context &
Context::restore() {
  if (!restore_stack.empty()) {
    auto & data = restore_stack.back();
    globalAlpha = data.globalAlpha;
    imageSmoothingEnabled = data.imageSmoothingEnabled;
    shadowBlur = data.shadowBlur;
    shadowColor = data.shadowColor;
    shadowOffsetX = data.shadowOffsetX;
    shadowOffsetY = data.shadowOffsetY;
    currentPath = data.currentPath;
    clipPath = data.clipPath;
    lineWidth = data.lineWidth;
    fillStyle = data.fillStyle;
    strokeStyle = data.strokeStyle;
    font = data.font;
    textAlign = data.textAlign;
    textBaseline = data.textBaseline;
  }
  return *this;
}
