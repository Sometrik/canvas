#include "ContextQuartz2D.h"

#include <iostream>

using namespace canvas;
using namespace std;

Quartz2DSurface::Quartz2DSurface(const std::string & filename) : Surface(0, 0, 0, 0), is_screen(false) {
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
  colorspace = CGColorSpaceCreateDeviceRGB();
  if (img) {
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img));
  
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    unsigned int bitmapBytesPerRow = getActualWidth() * 4;
    unsigned int bitmapByteCount = bitmapBytesPerRow * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    gc = CGBitmapContextCreate(bitmapData,
                               getActualWidth(),
                               getActualHeight(),
                               8,
                               bitmapBytesPerRow,
                               colorspace,
                               (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast)); // | kCGBitmapByteOrder32Big);
    initialize();
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    CGImageRelease(img);
  } else {
    resize(16, 16, 16, 16);
    unsigned int bitmapBytesPerRow = getActualWidth() * 4;
    unsigned int bitmapByteCount = bitmapBytesPerRow * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
    
    gc = CGBitmapContextCreate(bitmapData,
                               getActualWidth(),
                               getActualHeight(),
                               8,
                               bitmapBytesPerRow,
                               colorspace,
                               kCGImageAlphaPremultipliedLast); // | kCGBitmapByteOrder32Big);
    initialize();
  }
}

Quartz2DSurface::Quartz2DSurface(const unsigned char * buffer, size_t size) : Surface(0, 0, 0, 0), is_screen(false) {
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
  colorspace = CGColorSpaceCreateDeviceRGB();
  if (img) {
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img));
  
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    unsigned int bitmapBytesPerRow = getActualWidth() * 4;
    unsigned int bitmapByteCount = bitmapBytesPerRow * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    gc = CGBitmapContextCreate(bitmapData,
                               getActualWidth(),
                               getActualHeight(),
                               8,
                               bitmapBytesPerRow,
                               colorspace,
                               (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast)); // | kCGBitmapByteOrder32Big);
    initialize();
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    CGImageRelease(img);
  } else {
    Surface::resize(16, 16, 16, 16);
    unsigned int bitmapBytesPerRow = getActualWidth() * 4;
    unsigned int bitmapByteCount = bitmapBytesPerRow * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
    
    gc = CGBitmapContextCreate(bitmapData,
                               getActualWidth(),
                               getActualHeight(),
                               8,
                               bitmapBytesPerRow,
                               colorspace,
                               kCGImageAlphaPremultipliedLast); // | kCGBitmapByteOrder32Big);
    initialize();
  }
}

void
Quartz2DSurface::sendPath(const Path & path) {
  float scale = path.getDisplayScale();
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
Quartz2DSurface::renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth) {
  switch (mode) {
  case STROKE:
    sendPath(path);
    CGContextSetRGBStrokeColor(gc, style.color.red,
			       style.color.green,
			       style.color.blue,
			       style.color.alpha);
    CGContextSetLineWidth(gc, lineWidth);
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
	
	CGGradientRef myGradient = CGGradientCreateWithColorComponents(colorspace, components, locations, num_locations);
	
	save();
	clip(path);
	
	CGPoint myStartPoint, myEndPoint;
	myStartPoint.x = style.x0;
	myStartPoint.y = style.y0;
	myEndPoint.x = style.x1;
	myEndPoint.y = style.y1;
	CGContextDrawLinearGradient(gc, myGradient, myStartPoint, myEndPoint, 0);
	restore();
	
	CGGradientRelease(myGradient);
      }
    } else {
      sendPath(path);
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
    CGSize myShadowOffset = CGSizeMake(shadowOffsetX * getDisplayScale(), shadowOffsetY * getDisplayScale());
    CGContextSaveGState(default_surface.gc);
    CGFloat cv[] = { shadowColor.red, shadowColor.green, shadowColor.blue, shadowColor.alpha };
    CGColorRef myColor = CGColorCreate(default_surface.colorspace, cv);
    CGContextSetShadowWithColor(default_surface.gc, myShadowOffset, getDisplayScale() * shadowBlur, myColor);
    getDefaultSurface().renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, getDisplayScale());
    CGContextRestoreGState(default_surface.gc);
  } else {
    getDefaultSurface().renderText(mode, font, style, textBaseline, textAlign, text, x, y, lineWidth, getDisplayScale());
  }
}

void
ContextQuartz2D::renderPath(RenderMode mode, const Style & style) {
  if (hasShadow()) {
    CGSize myShadowOffset = CGSizeMake(getDisplayScale() * shadowOffsetX, getDisplayScale() * shadowOffsetY);
    CGContextSaveGState(default_surface.gc);
    CGFloat cv[] = { shadowColor.red, shadowColor.green, shadowColor.blue, shadowColor.alpha };
    CGColorRef myColor = CGColorCreate(default_surface.colorspace, cv);
    CGContextSetShadowWithColor(default_surface.gc, myShadowOffset, getDisplayScale() * shadowBlur, myColor);
    getDefaultSurface().renderPath(mode, current_path, style, getDisplayScale() * lineWidth);
    CGContextRestoreGState(default_surface.gc);
    CGColorRelease(myColor);
  } else {
    getDefaultSurface().renderPath(mode, current_path, style, getDisplayScale() * lineWidth);
  }
}

void
ContextQuartz2D::drawImage(const Image & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    CGSize myShadowOffset = CGSizeMake(shadowOffsetX, shadowOffsetY);
    CGContextSaveGState(default_surface.gc);
    CGFloat cv[] = { shadowColor.red, shadowColor.green, shadowColor.blue, shadowColor.alpha };
    CGColorRef myColor = CGColorCreate(default_surface.colorspace, cv);
    CGContextSetShadowWithColor(default_surface.gc, myShadowOffset, getDisplayScale() * shadowBlur, myColor);
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
    CGContextRestoreGState(default_surface.gc);
  } else {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
  }
}

void
ContextQuartz2D::drawImage(Surface & img, double x, double y, double w, double h) {
  if (hasShadow()) {
    CGSize myShadowOffset = CGSizeMake(shadowOffsetX, shadowOffsetY);
    CGContextSaveGState(default_surface.gc);
    CGFloat cv[] = { shadowColor.red, shadowColor.green, shadowColor.blue, shadowColor.alpha };
    CGColorRef myColor = CGColorCreate(default_surface.colorspace, cv);
    CGContextSetShadowWithColor(default_surface.gc, myShadowOffset, getDisplayScale() * shadowBlur, myColor);
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
    CGContextRestoreGState(default_surface.gc);
  } else {
    getDefaultSurface().drawImage(img, x * getDisplayScale(), y * getDisplayScale(), w * getDisplayScale(), h * getDisplayScale(), globalAlpha, imageSmoothingEnabled);
  }
}

