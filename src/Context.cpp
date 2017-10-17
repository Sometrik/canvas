#include <Context.h>

#include <cmath>
#include <iostream>

using namespace std;
using namespace canvas;

void
Context::resize(unsigned int _width, unsigned int _height) {
  getDefaultSurface().resize(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), getDefaultSurface().getNumChannels());
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
  Path2D path;
  path.moveTo(currentTransform.multiply(x, y));
  path.lineTo(currentTransform.multiply(x + w, y));
  path.lineTo(currentTransform.multiply(x + w, y + h));
  path.lineTo(currentTransform.multiply(x, y + h));
  path.closePath();
  Style style(this);
  style = Color(0.0f, 0.0f, 0.0f, 0.0f);
  return renderPath(FILL, path, style, COPY);
}

Context &
Context::renderText(RenderMode mode, const Style & style, const std::string & text, const Point & p, Operator op) {
  if (hasNativeShadows()) {
    getDefaultSurface().renderText(mode, font, style, textBaseline.get(), textAlign.get(), text, p, lineWidth.get(), op, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath);
  } else {
    if (hasShadow()) {
      float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
      int bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);

      Style shadow_style(this);
      shadow_style = shadowColor.get();
      shadow_style.color.alpha = 1.0f;
      shadow->renderText(mode, font, shadow_style, textBaseline.get(), textAlign.get(), text, Point(p.x + shadowOffsetX.get() + b, p.y + shadowOffsetY.get() + b), lineWidth.get(), op, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath);
      auto shadow1 = shadow->blur(bs, bs);
      auto shadow2 = shadow1->colorize(shadowColor.get());
      getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
    }
    getDefaultSurface().renderText(mode, font, style, textBaseline.get(), textAlign.get(), text, p, lineWidth.get(), op, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath);
  }
  return *this;
}

Context &
Context::renderPath(RenderMode mode, const Path2D & path, const Style & style, Operator op) {
  if (hasNativeShadows()) {
    getDefaultSurface().renderPath(mode, path, style, lineWidth.get(), op, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath);
  } else {
    if (hasShadow()) {
      float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      Style shadow_style(this);
      shadow_style = shadowColor.get();
      Path2D tmp_path = path, tmp_clipPath = clipPath;
      tmp_path.offset(shadowOffsetX.get() + bi, shadowOffsetY.get() + bi);
      tmp_clipPath.offset(shadowOffsetX.get() + bi, shadowOffsetY.get() + bi);
      
      shadow->renderPath(mode, tmp_path, shadow_style, lineWidth.get(), op, getDisplayScale(), globalAlpha.get(), 0, 0, 0, shadowColor.get(), tmp_clipPath);
      auto shadow1 = shadow->blur(bs, bs);
      auto shadow2 = shadow1->colorize(shadowColor.get());
      getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
    }
    getDefaultSurface().renderPath(mode, path, style, lineWidth.get(), op, getDisplayScale(), globalAlpha.get(), 0, 0, 0, shadowColor.get(), clipPath);
  }
  return *this;
}

Context &
Context::drawImage(Surface & img, double x, double y, double w, double h) {
  Point p = currentTransform.multiply(x, y);
  if (hasNativeShadows()) {
    getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath, imageSmoothingEnabled.get());
  } else {
    if (hasShadow()) {
      float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);

      shadow->drawImage(img, Point(p.x + b + shadowOffsetX.get(), p.y + b + shadowOffsetY.get()), w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
      // shadow->colorFill(shadowColor.get());
      auto shadow1 = shadow->blur(bs, bs);
      auto shadow2 = shadow1->colorize(shadowColor.get());
      getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
    }
    getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
  }
  return *this;
}

Context &
Context::drawImage(const ImageData & img, double x, double y, double w, double h) {
  Point p = currentTransform.multiply(x, y);
  if (hasNativeShadows()) {
    getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath, imageSmoothingEnabled.get());
  } else {
    if (hasShadow()) {
      float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
      float bi = int(ceil(b));
      auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, R8);
      shadow->drawImage(img, Point(x + b + shadowOffsetX.get(), y + b + shadowOffsetY.get()), w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
      // shadow->colorFill(shadowColor.get());
      auto shadow1 = shadow->blur(bs, bs);
      auto shadow2 = shadow1->colorize(shadowColor.get());
      getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
    }
    getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
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

TextMetrics
Context::measureText(const std::string & text) {
  return getDefaultSurface().measureText(font, text, textBaseline.get(), getDisplayScale());
}
