#include "ContextQuartz2D.h"

#include <iostream>

using namespace canvas;
using namespace std;

Quartz2DSurface::Quartz2DSurface(const std::string & filename) : Surface(0, 0) {
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
    resize(CGImageGetWidth(img), CGImageGetHeight(img));
  
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    unsigned int bitmapBytesPerRow = getWidth() * 4; // (has_alpha ? 4 : 3);
    unsigned int bitmapByteCount = bitmapBytesPerRow * getHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    gc = CGBitmapContextCreate(bitmapData,
                               getWidth(),
                               getHeight(),
                               8,
                               bitmapBytesPerRow,
                               colorspace,
                               has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast);
    assert(gc);
    CGContextDrawImage(gc, CGRectMake(0, 0, getWidth(), getHeight()), img);
    CGImageRelease(img);
  } else {
    resize(16, 16);
    unsigned int bitmapBytesPerRow = getWidth() * 4;
    unsigned int bitmapByteCount = bitmapBytesPerRow * getHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
    
    gc = CGBitmapContextCreate(bitmapData,
                               getWidth(),
                               getHeight(),
                               8,
                               bitmapBytesPerRow,
                               colorspace,
                               kCGImageAlphaPremultipliedLast);
  }
}

void
Quartz2DSurface::sendPath(const Path & path) {
  CGContextBeginPath(gc);
  for (auto pc : path.getData()) {
    switch (pc.type) {
    case PathComponent::MOVE_TO: CGContextMoveToPoint(gc, pc.x0, pc.y0); break;
    case PathComponent::LINE_TO: CGContextAddLineToPoint(gc, pc.x0, pc.y0); break;
    case PathComponent::ARC: CGContextAddArc(gc, pc.x0, pc.y0, pc.radius, pc.sa, pc.ea, pc.anticlockwise); break;
    case PathComponent::CLOSE: CGContextClosePath(gc); break;
    }
  }
}

void
Quartz2DSurface::fill(const Path & path, const Style & style) {
  sendPath(path);
  CGContextSetRGBFillColor(gc, style.color.red,
			   style.color.green,
			   style.color.blue,
			   style.color.alpha);
  CGContextFillPath(gc);
}

void
Quartz2DSurface::stroke(const Path & path, const Style & style, double lineWidth) {
  sendPath(path);
  CGContextSetRGBStrokeColor(gc, style.color.red,
			   style.color.green,
			   style.color.blue,
			   style.color.alpha);
  // CGContextSetLineWidth(context, fillStyle.);
  CGContextStrokePath(gc);  
}
