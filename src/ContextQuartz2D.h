#ifndef _CANVAS_CONTEXTQUARTZ2D_H_
#define _CANVAS_CONTEXTQUARTZ2D_H_

#include "Context.h"

#include <cassert>
#include <QuartzCore/QuartzCore.h>
#include <CoreText/CoreText.h>

#include <sstream>

namespace canvas {
  class Quartz2DFontCache {
  public:
    Quartz2DFontCache() { }
    ~Quartz2DFontCache() {
      for (auto it : fonts) {
        CFRelease(it.second);
      }
    }
    
    CTFontRef getFont(const Font & font, float display_scale) {
      float size = font.size * display_scale;
      bool is_bold = font.weight == Font::BOLD || font.weight == Font::BOLDER;
      bool is_italic = font.slant == Font::ITALIC;
      std::ostringstream key;
      key << font.family << "/" << size << "/" << (is_bold ? "bold" : "") << "/" << (is_italic ? "italic" : "");
      auto it = fonts.find(key.str());
      if (it != fonts.end()) {
        return it->second;
      } else {
        CTFontRef font2;
        if (is_italic && is_bold) {
          font2 = CTFontCreateWithName(CFSTR("Arial-BoldItalicMT"), size, NULL);
        } else if (is_bold) {
          font2 = CTFontCreateWithName(CFSTR("Arial-BoldMT"), size, NULL);
        } else if (is_italic) {
          font2 = CTFontCreateWithName(CFSTR("Arial-ItalicMT"), size, NULL);
        } else {
          font2 = CTFontCreateWithName(CFSTR("ArialMT"), size, NULL);
        }
        fonts[key.str()] = font2;
        return font2;
      }
    }
    
  private:
    std::map<std::string, CTFontRef> fonts;
  };
  
  class Quartz2DSurface : public Surface {
  public:
    friend class ContextQuartz2D;
        
  Quartz2DSurface(Quartz2DFontCache * _font_cache, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool has_alpha = true)
    : Surface(_logical_width, _logical_height, _actual_width, _actual_height),
      font_cache(_font_cache) {
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
  
  Quartz2DSurface(Quartz2DFontCache * _font_cache, const Image & image)
    : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight()),
      font_cache(_font_cache) {
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
    
    Quartz2DSurface(Quartz2DFontCache * _font_cache, const std::string & filename);
    Quartz2DSurface(Quartz2DFontCache * _font_cache, const unsigned char * buffer, size_t size);

    void initialize() {
      assert(gc);
      CGContextSetInterpolationQuality(gc, kCGInterpolationHigh);
      CGContextSetShouldAntialias(gc, true);
      CGContextTranslateCTM(gc, 0, getActualHeight());
      CGContextScaleCTM(gc, 1.0, -1.0);
    }
      
    ~Quartz2DSurface() {
      if (active_shadow_color) CGColorRelease(active_shadow_color);
      CGColorSpaceRelease(colorspace);
      delete[] bitmapData;
    }

    void * lockMemory(bool write_access = false) { return bitmapData; }
    
    void releaseMemory() { }

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, float display_scale) override;

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
      CTFontRef font2 = font_cache->getFont(font, display_scale);
      CGColorRef color = createCGColor(style.color);
      
#if 0
      int traits = 0;
      if (font.weight == Font::BOLD || font.weight == Font::BOLDER) traits |= kCTFontBoldTrait;
      if (font.slant == Font::ITALIC) traits |= kCTFontItalicTrait;
      CFNumberRef traits2 = CFNumberCreate(NULL, kCFNumberSInt32Type, &traits);
#endif
      
      CFStringRef text2 = CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingUTF8);
      CFStringRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName }; // kCTFontSymbolicTrait };
      CFTypeRef values[] = { font2, color }; // traits2
      
      CFDictionaryRef attr = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
      
      CFAttributedStringRef attrString = CFAttributedStringCreate(NULL, text2, attr);
      CTLineRef line = CTLineCreateWithAttributedString(attrString);
      
      x *= display_scale;
      y *= display_scale;

      if (textAlign.getType() != TextAlign::LEFT || textBaseline.getType() == TextBaseline::MIDDLE || textBaseline.getType() == TextBaseline::TOP) {
	CGFloat ascent, descent, leading;
	double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
	
	switch (textBaseline.getType()) {
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
      }
      
      // CGContextSetTextMatrix(gc, CGAffineTransformIdentity);
      CGContextSetTextMatrix(gc, CGAffineTransformMakeScale(1.0, -1.0)); // Use this one if the view's coordinates are flipped
      CGContextSetTextPosition(gc, x, y);
      CTLineDraw(line, gc);

      CFRelease(line);
      CFRelease(attrString);
      CFRelease(attr);
      CFRelease(text2);
      // CFRelease(traits2);
      CGColorRelease(color);
    }

    TextMetrics measureText(const Font & font, const std::string & text, float display_scale) {
      CTFontRef font2 = font_cache->getFont(font, display_scale);
      CFStringRef text2 = CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingUTF8);
      CFStringRef keys[] = { kCTFontAttributeName };
      CFTypeRef values[] = { font2 };
      CFDictionaryRef attr = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
      
      CFAttributedStringRef attrString = CFAttributedStringCreate(NULL, text2, attr);
      CTLineRef line = CTLineCreateWithAttributedString(attrString);
      
      CGFloat ascent, descent, leading;
      double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
      
      CFRelease(line);
      CFRelease(attrString);
      CFRelease(attr);
      CFRelease(text2);
      
      return TextMetrics(width / display_scale);
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
    
    void setShadow(float shadowOffsetX, float shadowOffsetY, float shadowBlur, const Color & shadowColor, float display_scale) {
      CGSize offset = CGSizeMake(display_scale * shadowOffsetX, display_scale * shadowOffsetY);
      if (active_shadow_color) CGColorRelease(active_shadow_color);
      active_shadow_color = createCGColor(shadowColor);
      CGContextSetShadowWithColor(gc, offset, display_scale * shadowBlur, active_shadow_color);
    }
    
  private:
    Quartz2DFontCache * font_cache;
    CGContextRef gc;
    CGColorSpaceRef colorspace;
    unsigned char * bitmapData;
    CGColorRef active_shadow_color = 0;
  };

  class ContextQuartz2D : public Context {
  public:
  ContextQuartz2D(Quartz2DFontCache * _font_cache, unsigned int _width, unsigned int _height, float _display_scale)
    : Context(_display_scale),
      font_cache(_font_cache),
      default_surface(_font_cache, _width, _height, (unsigned int)(_width * _display_scale), (unsigned int)(_height * _display_scale))
      {
      }

    std::shared_ptr<Surface> createSurface(const Image & image) {
        return std::shared_ptr<Surface>(new Quartz2DSurface(font_cache, image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, bool _has_alpha) {
      return std::shared_ptr<Surface>(new Quartz2DSurface(font_cache, _width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), _has_alpha));
    }
      std::shared_ptr<Surface> createSurface(const std::string & filename) {
          return std::shared_ptr<Surface>(new Quartz2DSurface(font_cache, filename));
      }
    void clearRect(double x, double y, double w, double h) { }
        
    Surface & getDefaultSurface() { return default_surface; }
    const Surface & getDefaultSurface() const { return default_surface; }
      
    void setgc(CGContextRef _gc) { default_surface.gc = _gc; }

    void drawImage(const Image & img, double x, double y, double w, double h) override;
    void drawImage(Surface & img, double x, double y, double w, double h) override;
    
  protected:
    void renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y) override;
    void renderPath(RenderMode mode, const Style & style) override;
    
  private:
    Quartz2DFontCache * font_cache;
    Quartz2DSurface default_surface;
  };

  class Quartz2DContextFactory : public ContextFactory {
  public:
    Quartz2DContextFactory(float _display_scale, std::shared_ptr<FilenameConverter> & _converter) : ContextFactory(_display_scale), converter(_converter) { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, bool apply_scaling = true) override {
      std::shared_ptr<Context> ptr(new ContextQuartz2D(&font_cache, width, height, apply_scaling ? getDisplayScale() : 1.0f));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      std::string filename2;
      if (converter->convert(filename, filename2)) {
        std::shared_ptr<Surface> ptr(new Quartz2DSurface(&font_cache, filename2));
        return ptr;
      } else {
        return createSurface(16, 16, false);
      }
    }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, bool has_alpha) override {
      std::shared_ptr<Surface> ptr(new Quartz2DSurface(&font_cache, width, height, (unsigned int)(width * getDisplayScale()), (unsigned int)(height * getDisplayScale()), has_alpha));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) override {
      std::shared_ptr<Surface> ptr(new Quartz2DSurface(&font_cache, buffer, size));
      return ptr;
    }
  private:
    Quartz2DFontCache font_cache;
    std::shared_ptr<FilenameConverter> converter;
  };
};

#endif
