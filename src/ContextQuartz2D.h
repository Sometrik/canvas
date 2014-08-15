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

  Quartz2DSurface(unsigned int _width, unsigned int _height, CGContextRef & _gc) :
    Surface(_width, _height), gc(_gc) {
      colorspace = CGColorSpaceCreateDeviceRGB();
    }
#if 0
    Quartz2DSurface(unsigned int _width, unsigned int _height, const unsigned char * _data) {
      assert(0);
    }
#endif
      
    ~Quartz2DSurface() {
      CGColorSpaceRelease(colorspace);
      CGContextRelease(gc);
    }

    unsigned char * lockMemory(bool write_access = false) {
        return nullptr;
/*
      return CGBitmapContextGetData(gc);
*/
    }
    
    void releaseMemory() {
      
    }

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

    CGContextRef gc;
    CGColorSpaceRef colorspace;
  };

  class ContextQuartz2D : public Context {
  public:
    // get context with UIGraphicsGetCurrentContext();
  ContextQuartz2D(unsigned int _width, unsigned int _height, CGContextRef & _gc) 
    : Context(_width, _height),
      default_surface(_width, _height, _gc)  {
      
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
      return std::shared_ptr<Surface>(new Quartz2DSurface(_width, _height, data));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height) {
      return std::shared_ptr<Surface>(new Quartz2DSurface(_width, _height));
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
#if 0
      CGFloat components[] = { strokeStyle.color.red / 255.0f,
			       strokeStyle.color.green / 255.0f,
			       strokeStyle.color.blue / 255.0f,
			       1.0 };
#endif
      CGColorRef color = CGColorCreate(default_surface.colorspace, components);
      CGContextSetStrokeColorWithColor(default_surface.gc, color);
      CGContextStrokePath(default_surface.gc);
      CGColorRelease(color);
    }

    void fill() {
#if 0
      CGFloat components[] = { strokeStyle.color.red / 255.0f,
			       strokeStyle.color.green / 255.0f,
			       strokeStyle.color.blue / 255.0f,
			       1.0 };
#endif
      CGColorRef color = CGColorCreate(default_surface.colorspace, components);
      CGContextSetFillColorWithColor(default_surface.gc, color);
      CGContextFillPath(default_surface.gc);
      CGColorRelease(color);
    }

    TextMetrics measureText(const std::string & text) {
      CGPoint startpt = CGContextGetTextPosition(default_surface.gc);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextInvisible); 
      CGContextShowText (default_surface.gc, s.c_str(), s.size());
      CGPoint endpt = CGContextGetTextPosition(default_surface.gc);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextFill);
      canvas::TextMetrics rvalue;
      rvalue.width =  endpt.x - startpt.x;
      return rvalue;
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
