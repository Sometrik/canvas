#ifndef _CANVAS_CONTEXTQUARTZ2D_H_
#define _CANVAS_CONTEXTQUARTZ2D_H_

#include "Context.h"

#include <cassert>
#include <QuartzCore/QuartzCore.h>

namespace canvas {
  class Quartz2DSurface : public Surface {
  public:
    friend class ContextQuartz2D;
        
  Quartz2DSurface(unsigned int _width, unsigned int _height, bool has_alpha = true) :
    Surface(_width, _height) {
      colorspace = CGColorSpaceCreateDeviceRGB();

      unsigned int bitmapBytesPerRow = _width * 4; // (has_alpha ? 4 : 3);
      unsigned int bitmapByteCount = bitmapBytesPerRow * _height;
      bitmapData = new unsigned char[bitmapByteCount];
      memset(bitmapData, 0, bitmapByteCount);
      
      gc = CGBitmapContextCreate(bitmapData,
				 _width,
				 _height,
				 8,
				 bitmapBytesPerRow,
				 colorspace,
                                 (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast) | kCGBitmapByteOrder32Big); // kCGImageAlphaNone);
      initialize();
    }
  
  Quartz2DSurface(unsigned int _width, unsigned int _height, CGContextRef  _gc, bool _is_screen) :
    Surface(_width, _height),
      bitmapData(0)
	{
      colorspace = CGColorSpaceCreateDeviceRGB();
       gc = _gc;
       is_screen = _is_screen;
    }
    Quartz2DSurface(const Image & image) : Surface(image.getWidth(), image.getHeight()) {
        colorspace = CGColorSpaceCreateDeviceRGB();

	bool has_alpha = image.hasAlpha();
	unsigned int bitmapBytesPerRow = getWidth() * 4;
        unsigned int bitmapByteCount = bitmapBytesPerRow * getHeight();
        bitmapData = new unsigned char[bitmapByteCount];
        memcpy(bitmapData, image.getData(), bitmapByteCount);
        
        gc = CGBitmapContextCreate(bitmapData,
                                   getWidth(),
                                   getHeight(),
                                   8,
                                   bitmapBytesPerRow,
                                   colorspace,
				   (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast) | kCGBitmapByteOrder32Big);
	initialize();
    }
    
    Quartz2DSurface(const std::string & filename);

    void initialize() {
      assert(gc);
      CGContextSetInterpolationQuality(gc, kCGInterpolationHigh);
      CGContextSetShouldAntialias(gc, true);
      CGContextTranslateCTM(gc, 0, getHeight());
      CGContextScaleCTM(gc, 1.0, -1.0);
    }
      
    ~Quartz2DSurface() {
      CGColorSpaceRelease(colorspace);
      // Do not release screen, iOS will do that
      if (!is_screen) CGContextRelease(gc);
      delete[] bitmapData;
    }

    void * lockMemory(bool write_access = false, unsigned int scaled_width = 0, unsigned int scaled_height = 0) {
      // this should not be done for other contexts
      void * ptr = CGBitmapContextGetData(gc);
      assert(ptr == bitmapData);
      return ptr;
    }
    
    void releaseMemory() {
      
    }

    void stroke(const Path & path, const Style & style, double lineWidth);
    void fill(const Path & path, const Style & style);
      
      Quartz2DSurface * copy() {
          auto img = createImage();
          return new Quartz2DSurface(*img);
      }
      
      void strokeText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) {
	CGContextSelectFont(gc, "Arial", font.size, kCGEncodingMacRoman);
	// CGContextSetTextDrawingMode(gc, kCGTextStroke);
	CGAffineTransform xform = CGAffineTransformMake( 1.0,  0.0,
							 0.0, -1.0,
							 0.0,  0.0 );
	CGContextSetTextMatrix(gc, xform);
	CGContextSetTextDrawingMode(gc, kCGTextStroke);
	CGContextShowTextAtPoint(gc, (int)x, (int)y, text.c_str(), text.size());
      }
      
      void fillText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) {
#if 1
	CGContextSelectFont(gc, "Arial", font.size, kCGEncodingMacRoman);
	// CGContextSetTextDrawingMode(gc, kCGTextFill);
	CGAffineTransform xform = CGAffineTransformMake( 1.0,  0.0,
							 0.0, -1.0,
							 0.0,  0.0 );
	CGContextSetTextMatrix(gc, xform);
	CGContextSetTextDrawingMode(gc, kCGTextFill);
	CGContextShowTextAtPoint(gc, (int)x, (int)y, text.c_str(), text.size());
#else
      // Prepare font
      CTFontRef font = CTFontCreateWithName(CFSTR("Times"), 20, NULL);

      CFStringRef text2 = CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingUTF8);

      // Create an attributed string
      CFStringRef keys[] = { kCTFontAttributeName };
      CFTypeRef values[] = { font };
      CFDictionaryRef attr = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
      CFAttributedStringRef attrString = CFAttributedStringCreate(NULL, text2, attr);
      CFRelease(attr);

      // Draw the string
      CTLineRef line = CTLineCreateWithAttributedString(attrString);
      CGContextSetTextMatrix(context, CGAffineTransformIdentity);  // Use this one when using standard view coordinates
      // CGContextSetTextMatrix(context, CGAffineTransformMakeScale(1.0, -1.0)); //Use this one if the view's coordinates are flipped
      
      CGContextSetTextPosition(context, x, y);
      CTLineDraw(line, context);

      // Clean up
      CFRelease(line);
      CFRelease(attrString);
      CFRelease(font);
      CFRelease(text2);
#endif
    }
    void drawImage(Surface & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) {
      assert(gc);
      Quartz2DSurface & img = dynamic_cast<Quartz2DSurface &>(_img);
      assert(img.gc);
      CGImageRef myImage = CGBitmapContextCreateImage(img.gc);
      CGContextDrawImage(gc, CGRectMake(x, y, w, h), myImage);
      CGImageRelease(myImage);
    }
    void clip(const Path & path) {
      sendPath(path);
      CGContextClip(gc);
    }
    void save() {
      
    }
    void restore() {

    }

  protected:
    void sendPath(const Path & path);

  private:
    CGContextRef gc;
    CGColorSpaceRef colorspace;
    bool is_screen;
      unsigned char * bitmapData;
  };

  class ContextQuartz2D : public Context {
  public:
    // get context with UIGraphicsGetCurrentContext();
  ContextQuartz2D(unsigned int _width, unsigned int _height, CGContextRef _gc, bool is_screen)
    : Context(_width, _height),
      default_surface(_width, _height, _gc, is_screen)  {
          setgc(_gc);
    }
  ContextQuartz2D(unsigned int _width, unsigned int _height)
    : Context(_width, _height),
      default_surface(_width, _height)
      {
      }

    std::shared_ptr<Surface> createSurface(const Image & image) {
        return std::shared_ptr<Surface>(new Quartz2DSurface(image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new Quartz2DSurface(_width, _height));
    }
      std::shared_ptr<Surface> createSurface(const std::string & filename) {
          return std::shared_ptr<Surface>(new Quartz2DSurface(filename));
      }
    void clearRect(double x, double y, double w, double h) { }
        
    Surface & getDefaultSurface() { return default_surface; }
    const Surface & getDefaultSurface() const { return default_surface; }
      
    void setgc(CGContextRef _gc){
      default_surface.gc = _gc;
    }

    TextMetrics measureText(const std::string & text) {
      CGContextSelectFont(default_surface.gc, "Arial", font.size, kCGEncodingMacRoman);
      CGContextSetTextDrawingMode(context, kCGTextInvisible);
      CGPoint initPos = CGContextGetTextPosition(context);
      CGContextShowTextAtPoint(context, initPos.x, initPos.y, text.c_str(), text.size());
      // CGContextShowText (default_surface.gc, text.c_str(), text.size());
      CGPoint finalPos = CGContextGetTextPosition(context);
      return TextMetrics(finalPos.x - initPos.x, font.size);
    }        

  private:
    Quartz2DSurface default_surface;
  };

  class Quartz2DContextFactory : public ContextFactory {
  public:
    Quartz2DContextFactory() { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) const {
        std::shared_ptr<Context> ptr(new ContextQuartz2D(width, height));
        return ptr;
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) const {
        std::shared_ptr<Surface> ptr(new Quartz2DSurface(filename));
        return ptr;
    }
    virtual std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height) const {
        std::shared_ptr<Surface> ptr(new Quartz2DSurface(width, height, false));
        return ptr;
    }
  };

};

#endif
