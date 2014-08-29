#ifndef _CONTEXTQUARTZ2D_H_
#define _CONTEXTQUARTZ2D_H_

#include "Context.h"

#include <cassert>
#include <QuartzCore/QuartzCore.h>

namespace canvas {
  class Quartz2DSurface : public Surface {
  public:
  
  
      
  Quartz2DSurface(unsigned int _width, unsigned int _height) :
    Surface(_width, _height) {
      colorspace = CGColorSpaceCreateDeviceRGB();

      unsigned int bitmapBytesPerRow = _width * 4;
      unsigned int bitmapByteCount = bitmapBytesPerRow * _height;
      unsigned char * bitmapData = new unsigned char[bitmapByteCount];
      
      gc = CGBitmapContextCreate(bitmapData,
				 _width,
				 _height,
				 8,
				 bitmapBytesPerRow,
				 colorspace,
				 kCGImageAlphaPremultipliedLast);      
    }
  
  Quartz2DSurface(unsigned int _width, unsigned int _height, CGContextRef  _gc, bool _is_screen) :
    Surface(_width, _height){
      colorspace = CGColorSpaceCreateDeviceRGB();
       gc = _gc;
       is_screen = _is_screen;
    }
#if 0
    Quartz2DSurface(unsigned int _width, unsigned int _height, const unsigned char * _data) {
      assert(0);
    }
#endif

      
    ~Quartz2DSurface() {
      CGColorSpaceRelease(colorspace);
      // Do not release screen, iOS will do that
      if (!is_screen) CGContextRelease(gc);
    }

    unsigned char * lockMemory(bool write_access = false) {
        return nullptr;
/*
      return CGBitmapContextGetData(gc);
*/
    }
    
    void releaseMemory() {
      
    }
    CGContextRef gc;
    CGColorSpaceRef colorspace;
      
  protected:
    void fillText(Context & context, const std::string & text, double x, double y) {
#if 0
      CGContextSelectFont(gc, "Arial", context.font.size, kCGEncodingMacRoman);
      CGContextSetTextDrawingMode(gc, kCGTextFill);
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
    void drawImage(Surface & _img, double x, double y, double w, double h) {
      Quartz2DSurface & img = dynamic_cast<Quartz2DSurface &>(_img);
#if 0
      CGRect myBoundingBox = CGRectMake (100, 100, 100, 100);
      CGImageRef myImage = CGBitmapContextCreateImage(surface.gc);
      CGContextDrawImage(gc, myBoundingBox, myImage);
      CGImageRelease(myImage);
#endif
    }

  private:
      bool is_screen;

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
#if 0
    ~ContextQuartz2D() {
    }
#endif

    virtual std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const unsigned char * data) {
        return std::shared_ptr<Surface>(new Quartz2DSurface(_width, _height)); //, data));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new Quartz2DSurface(_width, _height));
    }
   
    void fillRect(double x, double y, double w, double h) {
        CGContextSetRGBFillColor(default_surface.gc, strokeStyle.color.red / 255.0f,
                                 strokeStyle.color.green / 255.0f,
                                 strokeStyle.color.blue / 255.0f,
                                 1.0); 
        CGContextFillRect(default_surface.gc, CGRectMake(x, y, w, h));
    }
    
    void beginPath() {
      CGContextBeginPath(default_surface.gc);
    }
    void closePath() {
      CGContextClosePath(default_surface.gc);
    }
    void clip() {
      CGContextClip(default_surface.gc);
    }    
    void save() {
      
    }
    void restore() {

    }
    void arc(double x, double y, double r, double a0, double a1, bool t = false) {

    }
    void clearRect(double x, double y, double w, double h) {

    }
        
    Surface & getDefaultSurface() { return default_surface; }
    const Surface & getDefaultSurface() const { return default_surface; }

    void moveTo(double x0, double y0) {
      CGContextMoveToPoint(default_surface.gc, x0, y0);
    }
    void lineTo(double x0, double y0) {
      CGContextAddLineToPoint(default_surface.gc, x0, y0);
    }    
    void stroke() {
      // CGContextSetLineWidth(context, fillStyle.);

      CGFloat components[] = { strokeStyle.color.red / 255.0f,
			       strokeStyle.color.green / 255.0f,
			       strokeStyle.color.blue / 255.0f,
			       1.0 };

      CGColorRef color = CGColorCreate(default_surface.colorspace, components);
      CGContextSetStrokeColorWithColor(default_surface.gc, color);
      CGContextStrokePath(default_surface.gc);
      CGColorRelease(color);
    }
      
    void setgc(CGContextRef _gc){
          default_surface.gc = _gc;
    }
    void fill() {

      CGFloat components[] = { strokeStyle.color.red / 255.0f,
			       strokeStyle.color.green / 255.0f,
			       strokeStyle.color.blue / 255.0f,
			       1.0 };

      CGColorRef color = CGColorCreate(default_surface.colorspace, components);
      CGContextSetFillColorWithColor(default_surface.gc, color);
      CGContextFillPath(default_surface.gc);
      CGColorRelease(color);
    }

    TextMetrics measureText(const std::string & text) {
#if 0
      CGPoint startpt = CGContextGetTextPosition(default_surface.gc);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextInvisible); 
      CGContextShowText (default_surface.gc, text.c_str(), text.size());
      CGPoint endpt = CGContextGetTextPosition(default_surface.gc);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextFill);
      canvas::TextMetrics rvalue;
      rvalue.width =  endpt.x - startpt.x;
      return rvalue;
#else
      return TextMetrix(0, 0);
#endif
    }

  protected:
    Point getCurrentPoint() {
      return Point(0, 0); // FIXME!
    }

  private:
    Quartz2DSurface default_surface;
  };
};

#endif
