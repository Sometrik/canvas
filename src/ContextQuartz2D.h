#ifndef _CONTEXTQUARTZ2D_H_
#define _CONTEXTQUARTZ2D_H_

#include "Context.h"

#include <cassert>

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
    
    Quartz2DSurface(unsigned int _width, unsigned int _height, const unsigned char * _data) {
      assert(0);
    }
    
    ~Quartz2DSurface() {
      CGColorSpaceRelease(colorspace);
      CGContextRelease(gc);
    }

    unsigned char * lockMemory() {
      return CGBitmapContextGetData(gc);
    }
    
    void releaseMemory() {
      
    }

  protected:
    void fillText(Context & context, const std::string & text, double x, double y) {
      CGContextSelectFont(gc, "Arial", context.font.size, kCGEncodingMacRoman);
      CGContextSetTextDrawingMode(gc, kCGTextFill);
      CGAffineTransform xform = CGAffineTransformMake( 1.0,  0.0,
						       0.0, -1.0,
						       0.0,  0.0 );
      CGContextSetTextMatrix(gc, xform);
      CGContextSetTextDrawingMode(gc, kCGTextFill); 
      
      CGContextShowTextAtPoint(gc, (int)x, (int)y, text.c_str(), text.size());
    }

    CGContextRef gc;
    CGColorSpaceRef colorspace;
  };

  class ContextQuartz2D : public Context {
  public:
    // get context with UIGraphicsGetCurrentContext();
  ContextQuartz2D(unsigned int _width, unsigned int _height, CGContextRef & _gc) 
    : Context(_width, _height),
      default_context(_width, _height, _gc) {
      
    }
  ContextQuartz2D(unsigned int _width, unsigned int _height)
    : Context(_width, _height),
      default_context(_width, _height)
      {
      }
    ~ContextQuartz2D() {
    }
    void moveTo(double x0, double y0) {
      CGContextMoveToPoint(default_surface.gc, x0, y0);
    }
    void lineTo(double x0, double y0) {
      CGContextAddLineToPoint(default_surface.gc, x0, y0);
    }

    void beginPath() {
      CGContextBeginPath(default_surface.gc);
    }
   
    void clip() {
      CGContextClip(default_surface.gc);
    }
    
    void CGContext2D::closePath() {
       CGContextClosePath(default_surface.gc);
    }
    void stroke() {
      // CGContextSetLineWidth(context, fillStyle.);
      CGFloat components[] = { strokeStyle.color.red / 255.0f,
			       strokeStyle.color.green / 255.0f,
			       strokeStyle.color.blue / 255.0f
			       1.0 };
      CGColorRef color = CGColorCreate(default_surface.colorspace, components);
      CGContextSetStrokeColorWithColor(default_surface.gc, color);
      CGContextStrokePath(default_surface.gc);
      CGColorRelease(color);
    }

    void fill() {
      CGFloat components[] = { strokeStyle.color.red / 255.0f,
			       strokeStyle.color.green / 255.0f,
			       strokeStyle.color.blue / 255.0f
			       1.0 };
      CGColorRef color = CGColorCreate(default_surface.colorspace, components);
      CGContextSetFillColorWithColor(default_surface.gc, color);
      CGContextFillPath(default_surface.gc);
      CGColorRelease(color);
    }

    void drawImage(Surface & surface, ...) {
      myBoundingBox = CGRectMake (100, 100, 100, 100);
      CGImageRef myImage = CGBitmapContextCreateImage(surface.gc);
      CGContextDrawImage(default_surface.gc, myBoundingBox, myImage);
      CGImageRelease(myImage);
    }

    Size measureText(const std::string & text) {
      CGPoint startpt = CGContextGetTextPosition(default_surface.gc);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextInvisible); 
      CGContextShowText (default_surface.gc, s.c_str(), s.size());
      CGPoint endpt = CGContextGetTextPosition(default_surface.gc);
      CGContextSetTextDrawingMode(default_surface.gc, kCGTextFill); 
      return Size(endpt.x - startpt.x, 0);
    }

  private:
    Quartz2DSurface default_surface;
  };
};

#endif
