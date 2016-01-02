#include <Context.h>

#include <cmath>
#include <iostream>

using namespace std;
using namespace canvas;

void
Context::resize(unsigned int _width, unsigned int _height) {
  getDefaultSurface().resize(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), getDefaultSurface().getFormat());
  hit_regions.clear();
}

Context &
Context::fillRect(double x, double y, double w, double h) {
  beginPath().rect(x, y, w, h);
  return fill();
} 

Context &
Context::strokeRect(double x, double y, double w, double h) {
  beginPath().rect(x, y, w, h);
  return stroke();
}

Context &
Context::clearRect(double x, double y, double w, double h) {
  Path path;
  path.rect(x, y, w, h);
  Style style(this);
  style = Color(0.0f, 0.0f, 0.0f, 0.0f);
  return renderPath(FILL, path, style, COPY);
}

Context &
Context::renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op) {
  if (hasNativeShadows()) {
    getDefaultSurface().renderText(mode, font, style, textBaseline.getValue(), textAlign.getValue(), text, x, y, lineWidth.getValue(), op, getDisplayScale(), globalAlpha.getValue(), shadowBlur.getValue(), shadowOffsetX.getValue(), shadowOffsetY.getValue(), shadowColor.getValue(), clipPath);
  } else {
    if (hasShadow()) {
      float b = shadowBlur.getValue(), bs = shadowBlur.getValue() * getDisplayScale();
      int bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      auto shadow2 = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, RGBA8);

      Style shadow_style(this);
      shadow_style = shadowColor.getValue();
      shadow_style.color.alpha = 1.0f;
      shadow->renderText(mode, font, shadow_style, textBaseline.getValue(), textAlign.getValue(), text, x + shadowOffsetX.getValue() + bi, y + shadowOffsetY.getValue() + bi, lineWidth.getValue(), op, getDisplayScale(), globalAlpha.getValue(), 0.0f, 0.0f, 0.0f, shadowColor.getValue(), clipPath);
#if 1
      shadow->slowBlur(bs, bs);
#else
      shadow->blur(bs);
#endif
      shadow->colorize(shadowColor.getValue(), *shadow2);
      getDefaultSurface().drawImage(*shadow2, -bi, -bi, shadow2->getActualWidth(), shadow2->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.getValue(), Path(), false);
    }
    getDefaultSurface().renderText(mode, font, style, textBaseline.getValue(), textAlign.getValue(), text, x, y, lineWidth.getValue(), op, getDisplayScale(), globalAlpha.getValue(), 0.0f, 0.0f, 0.0f, shadowColor.getValue(), clipPath);
  }
  return *this;
}

Context &
Context::renderPath(RenderMode mode, const Path & path, const Style & style, Operator op) {
  if (hasNativeShadows()) {
    getDefaultSurface().renderPath(mode, path, style, lineWidth.getValue(), op, getDisplayScale(), globalAlpha.getValue(), shadowBlur.getValue(), shadowOffsetX.getValue(), shadowOffsetY.getValue(), shadowColor.getValue(), clipPath);
  } else {
    if (hasShadow()) {
      float b = shadowBlur.getValue(), bs = shadowBlur.getValue() * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      auto shadow2 = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, RGBA8);
      Style shadow_style(this);
      shadow_style = shadowColor.getValue();
      Path tmp_path = path, tmp_clipPath = clipPath;
      tmp_path.offset(shadowOffsetX.getValue() + bi, shadowOffsetY.getValue() + bi);
      tmp_clipPath.offset(shadowOffsetX.getValue() + bi, shadowOffsetY.getValue() + bi);
      
      shadow->renderPath(mode, tmp_path, shadow_style, lineWidth.getValue(), op, getDisplayScale(), globalAlpha.getValue(), 0, 0, 0, shadowColor.getValue(), tmp_clipPath);
#if 1
      shadow->slowBlur(bs, bs);
#else
      shadow->blur(bs);
#endif
      shadow->colorize(shadowColor.getValue(), *shadow2);
      getDefaultSurface().drawImage(*shadow2, -bi, -bi, shadow2->getActualWidth(), shadow2->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.getValue(), Path(), false);
    }
    getDefaultSurface().renderPath(mode, path, style, lineWidth.getValue(), op, getDisplayScale(), globalAlpha.getValue(), 0, 0, 0, shadowColor.getValue(), clipPath);
  }
  return *this;
}

Context &
Context::drawImage(Surface & img, double x, double y, double w, double h) {
  if (hasNativeShadows()) {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha.getValue(), shadowBlur.getValue(), shadowOffsetX.getValue(), shadowOffsetY.getValue(), shadowColor.getValue(), clipPath, imageSmoothingEnabled);
  } else {
    if (hasShadow()) {
      float b = shadowBlur.getValue(), bs = shadowBlur.getValue() * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      auto shadow2 = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, RGBA8);

      shadow->drawImage(img, (x + bi + shadowOffsetX.getValue()) * getDisplayScale(), (y + bi + shadowOffsetY.getValue()) * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha.getValue(), 0.0f, 0.0f, 0.0f, shadowColor.getValue(), clipPath, imageSmoothingEnabled);
      // shadow->colorFill(shadowColor.getValue());
#if 1
      shadow->slowBlur(bs, bs);
#else
      shadow->blur(bs);
#endif
      shadow->colorize(shadowColor.getValue(), *shadow2);
      getDefaultSurface().drawImage(*shadow2, -bi, -bi, shadow2->getActualWidth(), shadow2->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.getValue(), Path(), false);
    }
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha.getValue(), 0.0f, 0.0f, 0.0f, shadowColor.getValue(), clipPath, imageSmoothingEnabled);
  }
  return *this;
}

Context &
Context::drawImage(const Image & img, double x, double y, double w, double h) {
  if (hasNativeShadows()) {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha.getValue(), shadowBlur.getValue(), shadowOffsetX.getValue(), shadowOffsetY.getValue(), shadowColor.getValue(), clipPath, imageSmoothingEnabled);
  } else {
    if (hasShadow()) {
      float b = shadowBlur.getValue(), bs = shadowBlur.getValue() * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      auto shadow2 = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, RGBA8);

      shadow->drawImage(img, (x + bi + shadowOffsetX.getValue()) * getDisplayScale(), (y + bi + shadowOffsetY.getValue()) * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha.getValue(), 0.0f, 0.0f, 0.0f, shadowColor.getValue(), clipPath, imageSmoothingEnabled);
      // shadow->colorFill(shadowColor.getValue());
#if 1
      shadow->slowBlur(bs, bs);
#else
      shadow->blur(bs);
#endif
      shadow->colorize(shadowColor.getValue(), *shadow2);
      getDefaultSurface().drawImage(*shadow2, -bi, -bi, shadow2->getActualWidth(), shadow2->getActualHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.getValue(), Path(), false);
    }
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), getDisplayScale(), globalAlpha.getValue(), 0.0f, 0.0f, 0.0f, shadowColor.getValue(), clipPath, imageSmoothingEnabled);
  }
  return *this;
}

Context &
Context::save() {
  restore_stack.push_back(*this);
  return *this;
}

Context &
Context::restore() {
  if (!restore_stack.empty()) {
    *this = restore_stack.back();
    restore_stack.pop_back();    
  }
  return *this;
}
