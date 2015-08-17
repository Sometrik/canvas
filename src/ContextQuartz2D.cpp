#include "ContextQuartz2D.h"

#include <iostream>

using namespace canvas;
using namespace std;

Quartz2DSurface::Quartz2DSurface(Quartz2DFontCache * _font_cache, const std::string & filename)
  : Surface(0, 0, 0, 0, true), font_cache(_font_cache) {
  cerr << "trying to load file " << filename << endl;
  CGDataProviderRef provider = CGDataProviderCreateWithFilename(filename.c_str());
  CGImageRef img;
  if (filename.size() >= 4 && filename.compare(filename.size() - 4, 4, ".png") == 0) {
    img = CGImageCreateWithPNGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
  } else if (filename.size() >= 4 && filename.compare(filename.size() - 4, 4, ".jpg") == 0) {
    img = CGImageCreateWithJPEGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
  } else {
    cerr << "could not open file " << filename << endl;
    assert(0);
    img = 0;
  }
  CGDataProviderRelease(provider);
  if (img) {
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img), has_alpha);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    initializeContext();
    
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    CGImageRelease(img);
  } else {
    resize(16, 16, 16, 16, true);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  }
}

Quartz2DSurface::Quartz2DSurface(Quartz2DFontCache * _font_cache, const unsigned char * buffer, size_t size) : Surface(0, 0, 0, 0, true), font_cache(_font_cache) {
  CGDataProviderRef provider = CGDataProviderCreateWithData(0, buffer, size, 0);
  CGImageRef img;
  if (isPNG(buffer, size)) {
    img = CGImageCreateWithPNGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
  } else if (isJPEG(buffer, size)) {
    img = CGImageCreateWithJPEGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
  } else {
    cerr << "could not open" << endl;
    img = 0;
  }
  CGDataProviderRelease(provider);
  if (img) {
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img), has_alpha);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    initializeContext();
    
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    CGImageRelease(img);
  } else {
    Surface::resize(16, 16, 16, 16, true);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  }
}

void
Quartz2DSurface::sendPath(const Path & path, float scale) {
  initializeContext();
  CGContextBeginPath(gc);
  for (auto pc : path.getData()) {
    switch (pc.type) {
    case PathComponent::MOVE_TO: CGContextMoveToPoint(gc, pc.x0 * scale + 0.5, pc.y0 * scale + 0.5); break;
    case PathComponent::LINE_TO: CGContextAddLineToPoint(gc, pc.x0 * scale + 0.5, pc.y0 * scale + 0.5); break;
    case PathComponent::ARC: CGContextAddArc(gc, pc.x0 * scale + 0.5, pc.y0 * scale + 0.5, pc.radius * scale, pc.sa, pc.ea, pc.anticlockwise); break;
    case PathComponent::CLOSE: CGContextClosePath(gc); break;
    }
  }
}

void
Quartz2DSurface::renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale) {
  initializeContext();
  switch (mode) {
  case STROKE:
    sendPath(path, display_scale);
    CGContextSetRGBStrokeColor(gc, style.color.red,
			       style.color.green,
			       style.color.blue,
			       style.color.alpha);
    CGContextSetLineWidth(gc, lineWidth * display_scale);
    CGContextStrokePath(gc);  
    break;
  case FILL:
    if (style.getType() == Style::LINEAR_GRADIENT) {
      const std::map<float, Color> & colors = style.getColors();
      if (!colors.empty()) {
	std::map<float, Color>::const_iterator it0 = colors.begin(), it1 = colors.end();
	it1--;
	const Color & c0 = it0->second, c1 = it1->second;
	
	size_t num_locations = 2;
	CGFloat locations[2] = { 0.0, 1.0 };
	CGFloat components[8] = {
	  c0.red, c0.green, c0.blue, c0.alpha,
	  c1.red, c1.green, c1.blue, c1.alpha
	};
	
	CGGradientRef myGradient = CGGradientCreateWithColorComponents(cache->getColorSpace(), components, locations, num_locations);
	
	save();
	clip(path, display_scale);
	
	CGPoint myStartPoint, myEndPoint;
	myStartPoint.x = style.x0 * display_scale;
	myStartPoint.y = style.y0 * display_scale;
	myEndPoint.x = style.x1 * display_scale;
	myEndPoint.y = style.y1 * display_scale;
	CGContextDrawLinearGradient(gc, myGradient, myStartPoint, myEndPoint, 0);
	restore();
	
	CGGradientRelease(myGradient);
      }
    } else {
      sendPath(path, display_scale);
      CGContextSetRGBFillColor(gc, style.color.red,
			       style.color.green,
			       style.color.blue,
			       style.color.alpha);
      CGContextFillPath(gc);
    }
  }
}

void
ContextQuartz2D::renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, getDisplayScale());
    default_surface.restore();
  } else {
    getDefaultSurface().renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, getDisplayScale());
  }
}

void
ContextQuartz2D::renderPath(RenderMode mode, const Style & style, const Path & path, Operator op) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.renderPath(mode, path, style, lineWidth, op, getDisplayScale());
    default_surface.restore();
  } else {
    getDefaultSurface().renderPath(mode, path, style, lineWidth, op, getDisplayScale());
  }
}

void
ContextQuartz2D::drawImage(const Image & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
    default_surface.restore();
  } else {
    default_surface.drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
  }
}

void
ContextQuartz2D::drawImage(Surface & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
    default_surface.restore();
  } else {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
  }
}

