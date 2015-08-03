#ifndef _CANVAS_CONTEXTQUARTZ2D_H_
#define _CANVAS_CONTEXTQUARTZ2D_H_

#include "Context.h"

#include <cassert>
#include <QuartzCore/QuartzCore.h>
#include <CoreText/CoreText.h>

namespace canvas {
  class Quartz2DSurface : public Surface {
  public:
    friend class ContextQuartz2D;
        
  Quartz2DSurface(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool has_alpha = true)
    : Surface(_logical_width, _logical_height, _actual_width, _actual_height), is_screen(false) {
      colorspace = CGColorSpaceCreateDeviceRGB();

      assert(_logical_width && _logical_height);
      assert(_actual_width && _actual_height);
      
      unsigned int bitmapBytesPerRow = _actual_width * 4;
      unsigned int bitmapByteCount = bitmapBytesPerRow * _actual_height;
      bitmapData = new unsigned char[bitmapByteCount];
      memset(bitmapData, 0, bitmapByteCount);
      
      gc = CGBitmapContextCreate(bitmapData,
				 _actual_width,
				 _actual_height,
				 8,
				 bitmapBytesPerRow,
				 colorspace,
                                 (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast)); // kCGImageAlphaNone);  | kCGBitmapByteOrder32Big
      initialize();
    }
  
  Quartz2DSurface(unsigned int _width, unsigned int _height, CGContextRef  _gc, bool _is_screen) :
    Surface(_width, _height, _width, _height),
      bitmapData(0),
      is_screen(_is_screen)
	{
      colorspace = CGColorSpaceCreateDeviceRGB();
       gc = _gc;
    }
  Quartz2DSurface(const Image & image) : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight()), is_screen(false) {
        colorspace = CGColorSpaceCreateDeviceRGB();

	bool has_alpha = image.hasAlpha();
	unsigned int bitmapBytesPerRow = getActualWidth() * 4;
        unsigned int bitmapByteCount = bitmapBytesPerRow * getActualHeight();
        bitmapData = new unsigned char[bitmapByteCount];
        memcpy(bitmapData, image.getData(), bitmapByteCount);
        
        gc = CGBitmapContextCreate(bitmapData,
                                   getActualWidth(),
                                   getActualHeight(),
                                   8,
                                   bitmapBytesPerRow,
                                   colorspace,
                                   (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast)); //  | kCGBitmapByteOrder32Big);
	initialize();
    }
    
    Quartz2DSurface(const std::string & filename);
    Quartz2DSurface(const unsigned char * buffer, size_t size);

    void initialize() {
      assert(gc);
      CGContextSetInterpolationQuality(gc, kCGInterpolationHigh);
      CGContextSetShouldAntialias(gc, true);
      CGContextTranslateCTM(gc, 0, getActualHeight());
      CGContextScaleCTM(gc, 1.0, -1.0);
    }
      
    ~Quartz2DSurface() {
      CGColorSpaceRelease(colorspace);
      // Do not release screen, iOS will do that
      if (!is_screen) CGContextRelease(gc);
      delete[] bitmapData;
    }

    void * lockMemory(bool write_access = false) { return bitmapData; }
    
    void releaseMemory() { }

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, float display_scale) override;

#if 0
      Quartz2DSurface * copy() {
          auto img = createImage();
          return new Quartz2DSurface(*img);
      }
#endif

    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height) override {
      Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height);

      CGContextRelease(gc);
      delete[] bitmapData;

      bool has_alpha = true;
      unsigned int bitmapBytesPerRow = _actual_width * 4;
      unsigned int bitmapByteCount = bitmapBytesPerRow * _actual_height;
      bitmapData = new unsigned char[bitmapByteCount];
      memset(bitmapData, 0, bitmapByteCount);
      
      gc = CGBitmapContextCreate(bitmapData, _actual_width, _actual_height, 8, bitmapBytesPerRow, colorspace,
                                 (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast));
      initialize();
    }
    
    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, float display_scale) override {
      CTFontRef font2 = CTFontCreateWithName(CFSTR("ArialMT"), font.size * display_scale, NULL);
      CGColorRef color = createCGColor(style.color);
      CFStringRef text2 = CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingUTF8);
      CFStringRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
      CFTypeRef values[] = { font2, color };
      CFDictionaryRef attr = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
      
      CFAttributedStringRef attrString = CFAttributedStringCreate(NULL, text2, attr);
      CTLineRef line = CTLineCreateWithAttributedString(attrString);
      
      x *= display_scale;
      y *= display_scale;

      CGFloat ascent, descent, leading;
      double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);

      switch (textBaseline.getType()) {
        // case TextBaseline::MIDDLE: y -= (extents.height/2 + extents.y_bearing); break;
      case TextBaseline::MIDDLE: y += -descent + (ascent + descent) / 2.0; break;
      case TextBaseline::TOP: y += ascent; break;
      default: break;
      }
      
      switch (textAlign.getType()) {
        case TextAlign::LEFT: break;
        case TextAlign::CENTER: x -= width / 2; break;
        case TextAlign::RIGHT: x -= width; break;
        default: break;
      }
      
      // CGContextSetTextMatrix(gc, CGAffineTransformIdentity);
      CGContextSetTextMatrix(gc, CGAffineTransformMakeScale(1.0, -1.0)); // Use this one if the view's coordinates are flipped
      CGContextSetTextPosition(gc, x, y);
      CTLineDraw(line, gc);

      CFRelease(line);
      CFRelease(attrString);
      CFRelease(attr);
      CFRelease(font2);
      CFRelease(text2);
      CGColorRelease(color);
    }

    void drawImage(Surface & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) {
      Quartz2DSurface & img = dynamic_cast<Quartz2DSurface &>(_img);
      assert(img.gc);
      CGImageRef myImage = CGBitmapContextCreateImage(img.gc);
      CGContextDrawImage(gc, CGRectMake(x, y, w, h), myImage);
      CGImageRelease(myImage);
    }
    void drawImage(const Image & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) {
      CGDataProviderRef provider = CGDataProviderCreateWithData(0, _img.getData(), 4 * _img.getWidth() * _img.getHeight(), 0);
      CGImageRef img = CGImageCreate(_img.getWidth(), _img.getHeight(), 8, 32, 4 * _img.getWidth(), colorspace, (_img.hasAlpha() ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast),
                                     provider, 0, true, kCGRenderingIntentDefault);
      CGContextDrawImage(gc, CGRectMake(x, y, w, h), img);
      CGDataProviderRelease(provider);
      CGImageRelease(img);
    }
    void clip(const Path & path, float display_scale) {
      sendPath(path, display_scale);
      CGContextClip(gc);
    }
    void save() { CGContextSaveGState(gc); }
    void restore() { CGContextRestoreGState(gc); }

  protected:
    void renderPath(RenderMode mode, const Style & style);
    void sendPath(const Path & path, float display_scale);
    CGColorRef createCGColor(const Color & color) {
      CGFloat cv[] = { color.red, color.green, color.blue, color.alpha };
      return CGColorCreate(colorspace, cv);
    }

  private:
    CGContextRef gc;
    CGColorSpaceRef colorspace;
    bool is_screen;
      unsigned char * bitmapData;
  };

  class ContextQuartz2D : public Context {
  public:
    // get context with UIGraphicsGetCurrentContext();
  ContextQuartz2D(unsigned int _width, unsigned int _height, CGContextRef _gc, bool is_screen, float _display_scale)
    : Context(_display_scale),
      default_surface((unsigned int)(_width * _display_scale), (unsigned int)(_height * _display_scale), _gc, is_screen)  {
          setgc(_gc);
    }
  ContextQuartz2D(unsigned int _width, unsigned int _height, float _display_scale)
    : Context(_display_scale),
      default_surface(_width, _height, (unsigned int)(_width * _display_scale), (unsigned int)(_height * _display_scale))
      {
      }

    std::shared_ptr<Surface> createSurface(const Image & image) {
        return std::shared_ptr<Surface>(new Quartz2DSurface(image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new Quartz2DSurface(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale())));
    }
      std::shared_ptr<Surface> createSurface(const std::string & filename) {
          return std::shared_ptr<Surface>(new Quartz2DSurface(filename));
      }
    void clearRect(double x, double y, double w, double h) { }
        
    Surface & getDefaultSurface() { return default_surface; }
    const Surface & getDefaultSurface() const { return default_surface; }
      
    void setgc(CGContextRef _gc) { default_surface.gc = _gc; }

    TextMetrics measureText(const std::string & text) {
#if 0
      CGContextSelectFont(default_surface.gc, "Arial", font.size * getDisplayScale(), kCGEncodingMacRoman);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextInvisible);
      CGPoint initPos = CGContextGetTextPosition(default_surface.gc);
      CGContextShowTextAtPoint(default_surface.gc, initPos.x, initPos.y, text.c_str(), text.size());
      // CGContextShowText (default_surface.gc, text.c_str(), text.size());
      CGPoint finalPos = CGContextGetTextPosition(default_surface.gc);
      return TextMetrics((finalPos.x - initPos.x) / getDisplayScale(), font.size);
#else
      CTFontRef font2 = CTFontCreateWithName(CFSTR("ArialMT"), font.size * getDisplayScale(), NULL);
      CFStringRef text2 = CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingUTF8);
      CFStringRef keys[] = { kCTFontAttributeName };
      CFTypeRef values[] = { font2 };
      CFDictionaryRef attr = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
      
      CFAttributedStringRef attrString = CFAttributedStringCreate(NULL, text2, attr);
      CTLineRef line = CTLineCreateWithAttributedString(attrString);
      
      CGFloat ascent, descent, leading;
      double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
      return TextMetrics(width / getDisplayScale(), font.size);
#endif
    }

    void drawImage(const Image & img, double x, double y, double w, double h) override;
    void drawImage(Surface & img, double x, double y, double w, double h) override;
    
  protected:
    void renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y) override;
    void renderPath(RenderMode mode, const Style & style) override;
    
  private:
    Quartz2DSurface default_surface;
  };

  class Quartz2DContextFactory : public ContextFactory {
  public:
    Quartz2DContextFactory(float _display_scale, std::shared_ptr<FilenameConverter> & _converter) : ContextFactory(_display_scale), converter(_converter) { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, bool apply_scaling = true) const {
      std::shared_ptr<Context> ptr(new ContextQuartz2D(width, height, apply_scaling ? getDisplayScale() : 1.0f));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) const {
      std::string filename2;
      if (converter->convert(filename, filename2)) {
        std::shared_ptr<Surface> ptr(new Quartz2DSurface(filename2));
        return ptr;
      } else {
        return createSurface(16, 16);
      }
    }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height) const {
      std::shared_ptr<Surface> ptr(new Quartz2DSurface(width, height, (unsigned int)(width * getDisplayScale()), (unsigned int)(height * getDisplayScale()), false));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) const {
      std::shared_ptr<Surface> ptr(new Quartz2DSurface(buffer, size));
      return ptr;
    }
  private:
    std::shared_ptr<FilenameConverter> converter;
  };
};

#endif
