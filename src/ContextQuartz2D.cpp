#include "ContextQuartz2D.h"

void
Quartz2DSurface::sendPath(const Path & path) {
  CGContextBeginPath(gc);
  for (auto pc : path.getData()) {
    switch (pc.type) {
    case PathComponent::MOVE_TO: CGContextMoveToPoint(gc, pc.x0, pc.y0); break;
    case PathComponent::LINE_TO: CGContextAddLineToPoint(gc, pc.x0, pc.y0); break;
    case PathComponent::ARC:
      assert(0);
      break;
    }
  }
}

void
Quartz2DSurface::fill(const Path & path, const Style & style) {
  sendPath(path);
#if 1
  CGContextSetRGBFillColor(gc, strokeStyle.color.red,
			   strokeStyle.color.green,
			   strokeStyle.color.blue,
			   strokeStyle.color.alpha); 
  CGContextFillPath(gc);
#else
  CGFloat components[] = { style.color.red,
			   style.color.green,
			   style.color.blue,
			   style.color.alpha };
  CGColorRef color = CGColorCreate(colorspace, components);
  CGContextSetFillColorWithColor(gc, color);
  CGContextFillPath(gc);
  CGColorRelease(color);
#else
}

void
Quartz2DSurface::stroke(const Path & path, const Style & style, double lineWidth) {
  // CGContextSetLineWidth(context, fillStyle.);
  
  CGFloat components[] = { style.color.red,
			   style.color.green,
			   style.color.blue,
			   style.color.alpha };
  
  CGColorRef color = CGColorCreate(colorspace, components);
  CGContextSetStrokeColorWithColor(gc, color);
  CGContextStrokePath(gc);
  CGColorRelease(color);
}
