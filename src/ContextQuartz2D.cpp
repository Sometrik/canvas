#include "ContextQuartz2D.h"

#include <iostream>

#include <ImageIO/ImageIO.h>

using namespace canvas;
using namespace std;

Quartz2DSurface::Quartz2DSurface(Quartz2DCache * _cache, const std::string & filename)
  : Surface(0, 0, 0, 0, true), cache(_cache) {
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
  if (img) {
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img), has_alpha);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    initializeContext();
    flipY();
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    CGImageRelease(img);
    flipY();
  } else {
    resize(16, 16, 16, 16, true);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  }
  CGDataProviderRelease(provider);
}

Quartz2DSurface::Quartz2DSurface(Quartz2DCache * _cache, const unsigned char * buffer, size_t size) : Surface(0, 0, 0, 0, true), cache(_cache) {
  CGImageRef img;
  if (0 && isPNG(buffer, size)) {
    CGDataProviderRef provider = CGDataProviderCreateWithData(0, buffer, size, 0);
    img = CGImageCreateWithPNGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
  } else if (0 && isJPEG(buffer, size)) {
    CGDataProviderRef provider = CGDataProviderCreateWithData(0, buffer, size, 0);
    img = CGImageCreateWithJPEGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
  } else if (1) { // isGIF(buffer, size)) {
    CFDataRef data = CFDataCreate(0, buffer, size);
    CFStringRef keys[] = { kCGImageSourceShouldCache };
    CFTypeRef values[] = { kCFBooleanFalse };
    CFDictionaryRef options = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    auto isrc = CGImageSourceCreateWithData(data, 0);
    img = CGImageSourceCreateImageAtIndex(isrc, 0, 0);
    CFRelease(data);
    CFRelease(isrc);
    CFRelease(options);
  } else {
    cerr << "unhandled image type 1 = " << (int)buffer[0] << " 2 = " << (int)buffer[1] << " 3 = " << (int)buffer[2] << " 4 = " << (int)buffer[3] << " 5 = " << (int)buffer[4] << " 6 = " << (int)buffer[5] << endl;
    assert(0);
  }
  if (img) {
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img), has_alpha);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    initializeContext();
    flipY();    
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    flipY();
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
ContextQuartz2D::renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, op, getDisplayScale());
    default_surface.restore();
  } else {
    getDefaultSurface().renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, op, getDisplayScale());
  }
}

void
ContextQuartz2D::renderPath(RenderMode mode, const Path & path, const Style & style, Operator op) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.renderPath(mode, path, style, lineWidth, op, getDisplayScale());
    default_surface.restore();
  } else {
    getDefaultSurface().renderPath(mode, path, style, lineWidth, op, getDisplayScale());
  }
}

Context &
ContextQuartz2D::drawImage(const Image & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
    default_surface.restore();
  } else {
    default_surface.drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
  }
  return *this;
}

Context &
ContextQuartz2D::drawImage(Surface & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    default_surface.save();
    default_surface.setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, getDisplayScale());
    default_surface.drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
    default_surface.restore();
  } else {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
  }
  return *this;
}

