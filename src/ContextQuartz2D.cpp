#include <ContextQuartz2D.h>

#include <iostream>
#include <cassert>

#include <ImageIO/ImageIO.h>

using namespace canvas;
using namespace std;

Quartz2DSurface::Quartz2DSurface(const std::shared_ptr<Quartz2DCache> & _cache, const ImageData & image)
  : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getNumChannels()), cache(_cache) {
  assert(getActualWidth() && getActualHeight());
  size_t bitmapByteCount;
  if (getNumChannels() == 1) {
    bitmapByteCount = getActualWidth() * getActualHeight();
  } else {
    bitmapByteCount = 4 * getActualWidth() * getActualHeight();
  }
  bitmapData = new unsigned char[bitmapByteCount];
  if (image.getNumChannels() == 1 || image.getNumChannels() == 4) {
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
Quartz2DSurface::renderPath(RenderMode mode, const Matrix & transformation, const Path2D & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, const std::vector<float> & lineDash) {
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
	myStartPoint.x = style.p0.x * display_scale;
	myStartPoint.y = style.p0.y * display_scale;
	myEndPoint.x = style.p1.x * display_scale;
	myEndPoint.y = style.p1.y * display_scale;
	CGContextDrawLinearGradient(gc, myGradient, myStartPoint, myEndPoint, 0);
	
        CGContextRestoreGState(gc);

#ifdef MEMDEBUG
	if (CFGetRetainCount(myGradient) != 1) cerr << "leaking memory 7!\n";
#endif
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
Quartz2DSurface::resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, unsigned int _num_channels) {
  Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _num_channels);
  
  if (gc) {
#ifdef MEMDEBUG
    if (CFGetRetainCount(gc) != 1) std::cerr << "leaking memory E!\n";
#endif
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
Quartz2DSurface::drawImage(const ImageData & input, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled) {
  CGBitmapInfo bitmapInfo = 0;
  CGColorSpaceRef colorspace = 0;
  if (input.getNumChannels() == 4) {
    bitmapInfo |= kCGImageAlphaPremultipliedFirst;
    bitmapInfo |= kCGBitmapByteOrder32Little;
    colorspace = cache->getColorSpace();
  } else if (input.getNumChannels() == 3) {
    bitmapInfo |= kCGImageAlphaNoneSkipFirst;
    bitmapInfo |= kCGBitmapByteOrder32Little;
    colorspace = cache->getColorSpace();
  } else if (input.getNumChannels() == 2) {
    bitmapInfo |= kCGImageAlphaPremultipliedFirst;    
  } else if (input.getNumChannels() == 1) {
    bitmapInfo |= kCGImageAlphaNone;
    colorspace = cache->getColorSpaceGray();
  } else {
    return;
  }
    
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
  int bitsPerComponent = 8;
  int bitsPerPixel = input.getNumChannels() * bitsPerComponent;
  auto cfdata = CFDataCreate(0, input.getData(), input.getHeight() * input.getBytesPerRow());
  auto provider = CGDataProviderCreateWithCFData(cfdata);
  auto img = CGImageCreate(input.getWidth(), input.getHeight(), bitsPerComponent, bitsPerPixel, input.getBytesPerRow(), colorspace, bitmapInfo, provider, 0, imageSmoothingEnabled, kCGRenderingIntentDefault);
  if (img) {
    flipY();
    if (globalAlpha < 1.0f) CGContextSetAlpha(gc, globalAlpha);
    CGContextDrawImage(gc, CGRectMake(displayScale * p.x, getActualHeight() - 1 - displayScale * (p.y + h), displayScale * w, displayScale * h), img);
    if (globalAlpha < 1.0f) CGContextSetAlpha(gc, 1.0f);
    flipY();
  
#ifdef MEMDEBUG
    if (CFGetRetainCount(img) != 1) std::cerr << "leaking memory O!\n";
#endif
    CGImageRelease(img);
#ifdef MEMDEBUG
    if (CFGetRetainCount(provider) != 1) std::cerr << "leaking memory P!\n";
#endif
  } else {
    cerr << "failed to create CGImage\n";
  }
  CGDataProviderRelease(provider);
  CFRelease(cfdata);
  if (has_shadow || !clipPath.empty()) {
    CGContextRestoreGState(gc);
  }
}

class Quartz2DImage : public Image {
public:
  Quartz2DImage(FilenameConverter * _converter, float _display_scale)
    : Image(_display_scale), converter(_converter) { }

  Quartz2DImage(FilenameConverter * _converter, const std::string & _filename, float _display_scale)
    : Image(_filename, _display_scale),
      converter(_converter) { }

  Quartz2DImage(FilenameConverter * _converter, const unsigned char * _data, unsigned int _width, unsigned int _height, unsigned int _num_channels, float _display_scale) : Image(_data, _width, _height, _num_channels, _display_scale), converter(_converter) { }
  
  void loadFile() override {
#if 0
    // Create CFString
    CFStringRef filename2 = CFStringCreateWithCString(NULL, filename.c_str(), kCFStringEncodingUTF8);

    // Get a reference to the main bundle
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if (!mainBundle) {
      cerr << "failed to get main bundle, err = " << fnfErr << endl;
    }
    
    // Get a reference to the file's URL
    CFURLRef imageURL = CFBundleCopyResourceURL(mainBundle, filename2, NULL, NULL);

    // Convert the URL reference into a string reference
    CFStringRef imagePath = CFURLCopyFileSystemPath(imageURL, kCFURLPOSIXPathStyle);
    
    // Get the system encoding method
    CFStringEncoding encodingMethod = CFStringGetSystemEncoding();
    
    // Convert the string reference into a C string
    const char * tmp = CFStringGetCStringPtr(imagePath, encodingMethod);
    string path = tmp;

    CFRelease(imagePath);
    CFRelease(imageURL);
    CFRelease(mainBundle);
    CFRelease(filename2);
#else
    string path;
    converter->convert(filename, path);
#endif
    data = loadFromFile(path);
    if (!data.get()) filename.clear();
  }
      
    private:
      FilenameConverter * converter;
};

std::unique_ptr<Image>
Quartz2DContextFactory::loadImage(const std::string & filename) {
  return std::unique_ptr<Image>(new Quartz2DImage(converter, filename, getDisplayScale()));
}

std::unique_ptr<Image>
Quartz2DContextFactory::createImage() {
  return std::unique_ptr<Image>(new Quartz2DImage(converter, getDisplayScale()));
}

std::unique_ptr<Image>
Quartz2DContextFactory::createImage(const unsigned char * _data, unsigned int _width, unsigned int _height, unsigned int _num_channels) {
  return std::unique_ptr<Image>(new Quartz2DImage(converter, _data, _width, _height, _num_channels, getDisplayScale()));
}

std::unique_ptr<Image>
Quartz2DSurface::createImage(float display_scale) {
  unsigned char * buffer = (unsigned char *)lockMemory(false);
  assert(buffer);

  auto image = std::unique_ptr<Image>(new Quartz2DImage(0, buffer, getActualWidth(), getActualHeight(), getNumChannels(), display_scale));
  releaseMemory();
  
  return image;
}
