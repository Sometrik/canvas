#include <ContextQuartz2D.h>

#include <iostream>
#include <cassert>

#include <ImageIO/ImageIO.h>

using namespace canvas;
using namespace std;

Quartz2DSurface::Quartz2DSurface(Quartz2DCache * _cache, const Image & image)
  : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getImageFormat().hasAlpha() ? RGBA8 : RGB8), cache(_cache) {
  assert(getActualWidth() && getActualHeight());
  size_t bitmapByteCount = 4 * getActualWidth() * getActualHeight();
  bitmapData = new unsigned char[bitmapByteCount];
  if (image.getImageFormat().getBytesPerPixel() == 4) {
    memcpy(bitmapData, image.getData(), bitmapByteCount);
  } else {
    for (unsigned int i = 0; i < getActualWidth() * getActualHeight(); i++) {
      bitmapData[4 * i + 0] = image.getData()[3 * i + 2];
      bitmapData[4 * i + 1] = image.getData()[3 * i + 1];
      bitmapData[4 * i + 2] = image.getData()[3 * i + 0];
      bitmapData[4 * i + 3] = 255;
    }
  }
}

Quartz2DSurface::Quartz2DSurface(Quartz2DCache * _cache, const std::string & filename)
  : Surface(0, 0, 0, 0, RGBA8), cache(_cache) {
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
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img), has_alpha ? RGBA8 : RGB8);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    initializeContext();
    flipY();
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    if (CFGetRetainCount(img) != 1) cerr << "leaking memory 1!\n";
    CGImageRelease(img);
    flipY();
  } else {
    Surface::resize(16, 16, 16, 16, RGBA8);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  }
  if (CFGetRetainCount(provider) != 1) cerr << "leaking memory 2!\n";
  CGDataProviderRelease(provider);
}

Quartz2DSurface::Quartz2DSurface(Quartz2DCache * _cache, const unsigned char * buffer, size_t size) : Surface(0, 0, 0, 0, RGBA8), cache(_cache) {
  CGImageRef img = 0;
  CGDataProviderRef provider = 0;
  if (isPNG(buffer, size)) {
    provider = CGDataProviderCreateWithData(0, buffer, size, 0);
    img = CGImageCreateWithPNGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
  } else if (isJPEG(buffer, size)) {
    provider = CGDataProviderCreateWithData(0, buffer, size, 0);
    img = CGImageCreateWithJPEGDataProvider(provider, 0, false, kCGRenderingIntentDefault);
  } else if (isGIF(buffer, size) || isBMP(buffer, size)) {
    CFDataRef data = CFDataCreate(0, buffer, size);
    CFStringRef keys[3] = { kCGImageSourceShouldCache, kCGImageSourceCreateThumbnailFromImageIfAbsent, kCGImageSourceCreateThumbnailFromImageAlways };
    CFTypeRef values[3] = { kCFBooleanFalse, kCFBooleanFalse, kCFBooleanFalse };
    CFDictionaryRef options = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, 3, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    // sizeof(keys) / sizeof(keys[0])
    auto isrc = CGImageSourceCreateWithData(data, options);
    img = CGImageSourceCreateImageAtIndex(isrc, 0, options);
    if (CFGetRetainCount(isrc) != 1) cerr << "leaking memory 3!\n";
    CFRelease(isrc);
    if (CFGetRetainCount(options) != 1) cerr << "leaking memory 4!\n";
    CFRelease(options);
    long data_retain = CFGetRetainCount(data);
    if (data_retain != 1) cerr << "leaking memory 5 (" << data_retain << ")!\n";
    CFRelease(data);
  } else if (isXML(buffer, size)) {
    cerr << "trying to render XML/HTML" << endl;
    assert(0);
  } else {
    cerr << "unhandled image type 1 = " << (int)buffer[0] << " 2 = " << (int)buffer[1] << " 3 = " << (int)buffer[2] << " 4 = " << (int)buffer[3] << " 5 = " << (int)buffer[4] << " 6 = " << (int)buffer[5] << endl;
    assert(0);
  }
  if (img) {
    bool has_alpha = CGImageGetAlphaInfo(img) != kCGImageAlphaNone;
    Surface::resize(CGImageGetWidth(img), CGImageGetHeight(img), CGImageGetWidth(img), CGImageGetHeight(img), has_alpha ? RGBA8 : RGB8);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  
    initializeContext();
    flipY();    
    CGContextDrawImage(gc, CGRectMake(0, 0, getActualWidth(), getActualHeight()), img);
    flipY();
  } else {
    Surface::resize(16, 16, 16, 16, RGBA8);
    unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
    bitmapData = new unsigned char[bitmapByteCount];
    memset(bitmapData, 0, bitmapByteCount);
  }
  if (img) {
    if (CFGetRetainCount(img) != 1) cerr << "leaking CGImage!\n";
    CGImageRelease(img);
  }
  if (provider) {
    if (CFGetRetainCount(provider) != 1) cerr << "leaking CGDataProvider!\n";
    CGDataProviderRelease(provider);
  }
}

void
Quartz2DSurface::sendPath(const Path2D & path, float scale) {
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
Quartz2DSurface::renderPath(RenderMode mode, const Path2D & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) {
  initializeContext();

  bool has_shadow = shadowBlur > 0.0f || shadowOffsetX != 0.0f || shadowOffsetY != 0.0f;
  if (has_shadow || !clipPath.empty()) {
    CGContextSaveGState(gc);
  }
  if (!clipPath.empty()) {
    sendPath(clipPath, display_scale);
    CGContextClip(gc);
  }
  if (has_shadow) {
    setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, display_scale);
  }
  
  switch (op) {
  case SOURCE_OVER:
    CGContextSetBlendMode(gc, kCGBlendModeNormal);
    break;
  case COPY:
    CGContextSetBlendMode(gc, kCGBlendModeCopy);
    break;
  }
  switch (mode) {
  case STROKE:
    sendPath(path, display_scale);
    CGContextSetRGBStrokeColor(gc, style.color.red,
			       style.color.green,
			       style.color.blue,
			       style.color.alpha * globalAlpha);
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
	  c0.red, c0.green, c0.blue, c0.alpha * globalAlpha,
	  c1.red, c1.green, c1.blue, c1.alpha * globalAlpha
	};
	
	CGGradientRef myGradient = CGGradientCreateWithColorComponents(cache->getColorSpace(), components, locations, num_locations);
	
        CGContextSaveGState(gc);
        sendPath(path, display_scale);
        CGContextClip(gc);
	
	CGPoint myStartPoint, myEndPoint;
	myStartPoint.x = style.x0 * display_scale;
	myStartPoint.y = style.y0 * display_scale;
	myEndPoint.x = style.x1 * display_scale;
	myEndPoint.y = style.y1 * display_scale;
	CGContextDrawLinearGradient(gc, myGradient, myStartPoint, myEndPoint, 0);
	
        CGContextRestoreGState(gc);

	if (CFGetRetainCount(myGradient) != 1) cerr << "leaking memory 7!\n";
	CGGradientRelease(myGradient);
      }
    } else {
      sendPath(path, display_scale);
      CGContextSetRGBFillColor(gc, style.color.red,
			       style.color.green,
			       style.color.blue,
			       style.color.alpha * globalAlpha);
      CGContextFillPath(gc);
    }
  }
  if (op != SOURCE_OVER) {
    CGContextSetBlendMode(gc, kCGBlendModeNormal);
  }
  if (has_shadow || !clipPath.empty()) {
    CGContextRestoreGState(gc);
  }
}

void
Quartz2DSurface::resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format) {
  Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _format);
  
  if (gc) {
    if (CFGetRetainCount(gc) != 1) std::cerr << "leaking memory E!\n";
    CGContextRelease(gc);
    gc = 0;
  }
  delete[] bitmapData;
  
  assert(getActualWidth() && getActualHeight());
  unsigned int bitmapByteCount = 4 * getActualWidth() * getActualHeight();
  bitmapData = new unsigned char[bitmapByteCount];
  memset(bitmapData, 0, bitmapByteCount);
}

void
Quartz2DSurface::drawImage(const Image & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled) {
  initializeContext();
  bool has_shadow = shadowBlur > 0.0f || shadowOffsetX != 0.0f || shadowOffsetY != 0.0f;
  if (has_shadow || !clipPath.empty()) {
    CGContextSaveGState(gc);
  }
  if (!clipPath.empty()) {
    sendPath(clipPath, displayScale);
    CGContextClip(gc);
  }
  if (has_shadow) {
    setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, displayScale);
  }
  auto format = _img.getImageFormat();
  CGDataProviderRef provider = CGDataProviderCreateWithData(0, _img.getData(), format.getBytesPerPixel() * _img.getWidth() * _img.getHeight(), 0);
  assert(format.getBytesPerPixel() == 4);
  auto f = (format.hasAlpha() ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast);
  CGImageRef img = CGImageCreate(_img.getWidth(), _img.getHeight(), 8, format.getBytesPerPixel() * 8, format.getBytesPerPixel() * _img.getWidth(), cache->getColorSpace(), f, provider, 0, imageSmoothingEnabled, kCGRenderingIntentDefault);
  assert(img);
  flipY();
  if (globalAlpha < 1.0f) CGContextSetAlpha(gc, globalAlpha);
  CGContextDrawImage(gc, CGRectMake(displayScale * p.x, getActualHeight() - 1 - displayScale * (p.y + h), displayScale * w, displayScale * h), img);
  if (globalAlpha < 1.0f) CGContextSetAlpha(gc, 1.0f);
  flipY();
  
  if (CFGetRetainCount(img) != 1) std::cerr << "leaking memory O!\n";
  CGImageRelease(img);
  if (CFGetRetainCount(provider) != 1) std::cerr << "leaking memory P!\n";
  CGDataProviderRelease(provider);
  if (has_shadow || !clipPath.empty()) {
    CGContextRestoreGState(gc);
  }
}
