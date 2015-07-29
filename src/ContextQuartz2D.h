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
    Surface(_width, _height),
      is_screen(false) {
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
                                 (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast)); // kCGImageAlphaNone);  | kCGBitmapByteOrder32Big
      initialize();
    }
  
  Quartz2DSurface(unsigned int _width, unsigned int _height, CGContextRef  _gc, bool _is_screen) :
    Surface(_width, _height),
      bitmapData(0),
      is_screen(_is_screen)
	{
      colorspace = CGColorSpaceCreateDeviceRGB();
       gc = _gc;
    }
  Quartz2DSurface(const Image & image) : Surface(image.getWidth(), image.getHeight()), is_screen(false) {
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
                                   (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast)); //  | kCGBitmapByteOrder32Big);
	initialize();
    }
    
    Quartz2DSurface(const std::string & filename);
    Quartz2DSurface(const unsigned char * buffer, size_t size);

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
#if 0
      void * ptr = CGBitmapContextGetData(gc);
      assert(ptr == bitmapData);
      return ptr;
#else
      return bitmapData;
#endif
    }
    
    void releaseMemory() {
      
    }

    void stroke(const Path & path, const Style & style, double lineWidth);
    void fill(const Path & path, const Style & style);
      
      Quartz2DSurface * copy() {
          auto img = createImage();
          return new Quartz2DSurface(*img);
      }

    void resize(unsigned int _width, unsigned int _height) {
      Surface::resize(_width, _height);

      CGContextRelease(gc);
      delete[] bitmapData;

      bool has_alpha = true;
      unsigned int bitmapBytesPerRow = _width * 4;
      unsigned int bitmapByteCount = bitmapBytesPerRow * _height;
      bitmapData = new unsigned char[bitmapByteCount];
      memset(bitmapData, 0, bitmapByteCount);
      
      gc = CGBitmapContextCreate(bitmapData, _width, _height, 8, bitmapBytesPerRow, colorspace,
                                 (has_alpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast));
      initialize();
    }
      
      void strokeText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float display_scale) {
        CGContextSetRGBStrokeColor(gc, style.color.red,
                                 style.color.green,
                                 style.color.blue,
                                 style.color.alpha);
	CGContextSelectFont(gc, "Arial", font.size * display_scale, kCGEncodingMacRoman);
	// CGContextSetTextDrawingMode(gc, kCGTextStroke);
	CGAffineTransform xform = CGAffineTransformMake( 1.0,  0.0,
							 0.0, -1.0,
							 0.0,  0.0 );
	CGContextSetTextMatrix(gc, xform);
	CGContextSetTextDrawingMode(gc, kCGTextStroke);
	CGContextShowTextAtPoint(gc, (int)(x * display_scale), (int)(y * display_scale), text.c_str(), text.size());
      }
      
      void fillText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float display_scale) {
#if 1
        CGContextSetRGBFillColor(gc, style.color.red,
                                 style.color.green,
                                 style.color.blue,
                                 style.color.alpha);
	CGContextSelectFont(gc, "Arial", font.size * display_scale, kCGEncodingMacRoman);
	// CGContextSetTextDrawingMode(gc, kCGTextFill);
	CGAffineTransform xform = CGAffineTransformMake( 1.0,  0.0,
							 0.0, -1.0,
							 0.0,  0.0 );
	CGContextSetTextMatrix(gc, xform);
	CGContextSetTextDrawingMode(gc, kCGTextFill);
	CGContextShowTextAtPoint(gc, (int)(x * display_scale), (int)(y * display_scale), text.c_str(), text.size());
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
    void save() { CGContextSaveGState(gc); }
    void restore() { CGContextRestoreGState(gc); }

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
  ContextQuartz2D(unsigned int _width, unsigned int _height, CGContextRef _gc, bool is_screen, float _display_scale)
    : Context(_width, _height, _display_scale),
      default_surface(_width, _height, _gc, is_screen)  {
          setgc(_gc);
    }
  ContextQuartz2D(unsigned int _width, unsigned int _height, float _display_scale)
    : Context(_width, _height, _display_scale),
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
      
    void setgc(CGContextRef _gc) { default_surface.gc = _gc; }

    TextMetrics measureText(const std::string & text) {
      CGContextSelectFont(default_surface.gc, "Arial", font.size * getDisplayScale(), kCGEncodingMacRoman);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextInvisible);
      CGPoint initPos = CGContextGetTextPosition(default_surface.gc);
      CGContextShowTextAtPoint(default_surface.gc, initPos.x, initPos.y, text.c_str(), text.size());
      // CGContextShowText (default_surface.gc, text.c_str(), text.size());
      CGPoint finalPos = CGContextGetTextPosition(default_surface.gc);
      return TextMetrics((finalPos.x - initPos.x) / getDisplayScale(), font.size);
    }        

  private:
    Quartz2DSurface default_surface;
  };

  class Quartz2DContextFactory : public ContextFactory {
  public:
    Quartz2DContextFactory(float _display_scale, std::shared_ptr<FilenameConverter> & _converter) : ContextFactory(_display_scale), converter(_converter) { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height) const {
      std::shared_ptr<Context> ptr(new ContextQuartz2D(width, height, getDisplayScale()));
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
      std::shared_ptr<Surface> ptr(new Quartz2DSurface(width, height, false));
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
