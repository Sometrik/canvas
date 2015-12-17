#ifndef _CANVAS_CONTEXTQUARTZ2D_H_
#define _CANVAS_CONTEXTQUARTZ2D_H_

#include "Context.h"

#include <cassert>
#include <QuartzCore/QuartzCore.h>
#include <CoreText/CoreText.h>

#include <sstream>
#include <iostream>

namespace canvas {
  class Quartz2DCache {
  public:
    Quartz2DCache() { }
    ~Quartz2DCache() {
      if (colorspace && CFGetRetainCount(colorspace) != 1) std::cerr << "leaking memory A!\n";
      CGColorSpaceRelease(colorspace);
      for (auto & fd : fonts) {
	if (CFGetRetainCount(fd.second) != 1) std::cerr << "leaking memory B!\n";
        CFRelease(fd.second);
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
    
    CGColorSpaceRef & getColorSpace() {
      if (!colorspace) colorspace = CGColorSpaceCreateDeviceRGB();
      return colorspace;
    }
    
  private:
    CGColorSpaceRef colorspace = 0;
    std::map<std::string, CTFontRef> fonts;    
  };
  
  class Quartz2DSurface : public Surface {
  public:
    friend class ContextQuartz2D;
        
  Quartz2DSurface(Quartz2DCache * _cache, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format)
    : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format), cache(_cache) {
      if (_actual_width && _actual_height) {
        unsigned int bitmapBytesPerRow = _actual_width * 4;
        unsigned int bitmapByteCount = bitmapBytesPerRow * _actual_height;
        bitmapData = new unsigned char[bitmapByteCount];
        memset(bitmapData, 0, bitmapByteCount);
      }
  }
  
  Quartz2DSurface(Quartz2DCache * _cache, const Image & image)
    : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getFormat().hasAlpha() ? RGBA8 : RGB8), cache(_cache) {
      assert(getActualWidth() && getActualHeight());
      size_t bitmapByteCount = 4 * getActualWidth() * getActualHeight();
      bitmapData = new unsigned char[bitmapByteCount];
      if (image.getFormat().getBytesPerPixel() == 4) {
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
    
    Quartz2DSurface(Quartz2DCache * _cache, const std::string & filename);
    Quartz2DSurface(Quartz2DCache * _cache, const unsigned char * buffer, size_t size);
    
    ~Quartz2DSurface() {
      if (active_shadow_color) {
	if (CFGetRetainCount(active_shadow_color) != 1) std::cerr << "leaking memory C!\n";
	CGColorRelease(active_shadow_color);
      }
      if (gc) {
	if (CFGetRetainCount(gc) != 1) std::cerr << "leaking memory D!\n";
	CGContextRelease(gc);
      }
      delete[] bitmapData;
    }

    void * lockMemory(bool write_access = false) override { return bitmapData; }
    
    void releaseMemory() override { }

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor) override;

    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format) override {
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
    
    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor) override {
      initializeContext();

      bool has_shadow = shadowBlur > 0.0f || shadowOffsetX != 0.0f || shadowOffsetY != 0.0f;
      if (has_shadow) {
	save();
	setShadow(shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor, display_scale);
      }

      CFStringRef text2 = CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingUTF8);
      if (!text2) {
        std::cerr << "failed to create CString from '" << text << "'" << std::endl;
        return;
      }
      
      CTFontRef font2 = cache->getFont(font, display_scale);
      int font_retain = CFGetRetainCount(font2);
      if (font_retain != 1) std::cerr << "too many retains for font (" << font_retain << ")" << std::endl;
      CGColorRef color = createCGColor(style.color, globalAlpha);
      
#if 0
      int traits = 0;
      if (font.weight == Font::BOLD || font.weight == Font::BOLDER) traits |= kCTFontBoldTrait;
      if (font.slant == Font::ITALIC) traits |= kCTFontItalicTrait;
      CFNumberRef traits2 = CFNumberCreate(NULL, kCFNumberSInt32Type, &traits);
#endif
      
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

      if (CFGetRetainCount(line) != 1) std::cerr << "leaking memory F!\n";
      CFRelease(line);
      if (CFGetRetainCount(attrString) != 1) std::cerr << "leaking memory G!\n";
      CFRelease(attrString);
      if (CFGetRetainCount(attr) != 1) std::cerr << "leaking memory H!\n";
      CFRelease(attr);
      if (CFGetRetainCount(text2) != 1) std::cerr << "leaking memory I!\n";
      CFRelease(text2);
      // CFRelease(traits2);
      int color_retain = CFGetRetainCount(color);
      if (color_retain != 1) std::cerr << "leaking CGColor (" << color_retain << ")!\n";
      CGColorRelease(color);

      if (has_shadow) {
	restore();
      }
    }

    TextMetrics measureText(const Font & font, const std::string & text, float display_scale) override {
      CTFontRef font2 = cache->getFont(font, display_scale);
      CFStringRef text2 = CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingUTF8);
      CFStringRef keys[] = { kCTFontAttributeName };
      CFTypeRef values[] = { font2 };
      CFDictionaryRef attr = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
      
      CFAttributedStringRef attrString = CFAttributedStringCreate(NULL, text2, attr);
      CTLineRef line = CTLineCreateWithAttributedString(attrString);
      
      CGFloat ascent, descent, leading;
      double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
      
      if (CFGetRetainCount(line) != 1) std::cerr << "leaking memory K!\n";
      CFRelease(line);
      if (CFGetRetainCount(attrString) != 1) std::cerr << "leaking memory L!\n";
      CFRelease(attrString);
      if (CFGetRetainCount(attr) != 1) std::cerr << "leaking memory M!\n";
      CFRelease(attr);
      if (CFGetRetainCount(text2) != 1) std::cerr << "leaking memory N!\n";
      CFRelease(text2);
      
      return TextMetrics(width / display_scale);
    }

    void drawImage(Surface & surface, double x, double y, double w, double h, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, bool imageSmoothingEnabled = true) override {
      initializeContext();
#if 1
      auto img = surface.createImage();
      drawImage(*img, x, y, w, h, display_scale, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, imageSmoothingEnabled);
#else
      _img.initializeContext();
      Quartz2DSurface & img = dynamic_cast<Quartz2DSurface &>(_img);
      assert(img.gc);
      CGImageRef myImage = CGBitmapContextCreateImage(img.gc);
      CGContextDrawImage(gc, CGRectMake(x, y, w, h), myImage);
      CGImageRelease(myImage);
#endif
    }
    void drawImage(const Image & _img, double x, double y, double w, double h, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, bool imageSmoothingEnabled = true) override {
      initializeContext();
      auto & format = _img.getFormat();
      // std::cerr << "trying to draw image " << _img.getWidth() << " " << _img.getHeight() << " a=" << format.hasAlpha() << ", bpp=" << format.getBytesPerPixel() << std::endl;
      CGDataProviderRef provider = CGDataProviderCreateWithData(0, _img.getData(), format.getBytesPerPixel() * _img.getWidth() * _img.getHeight(), 0);
      assert(format.getBytesPerPixel() == 4);
      auto f = (format.hasAlpha() ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast);
      CGImageRef img = CGImageCreate(_img.getWidth(), _img.getHeight(), 8, format.getBytesPerPixel() * 8, format.getBytesPerPixel() * _img.getWidth(), cache->getColorSpace(), f, provider, 0, imageSmoothingEnabled, kCGRenderingIntentDefault);
      assert(img);
      flipY();
      if (globalAlpha < 1.0f) CGContextSetAlpha(gc, globalAlpha);
      CGContextDrawImage(gc, CGRectMake(x, getActualHeight() - 1 - y - h, w, h), img);
      if (globalAlpha < 1.0f) CGContextSetAlpha(gc, 1.0f);
      flipY();

      if (CFGetRetainCount(img) != 1) std::cerr << "leaking memory O!\n";
      CGImageRelease(img);
      if (CFGetRetainCount(provider) != 1) std::cerr << "leaking memory P!\n";
      CGDataProviderRelease(provider);
    }
    void clip(const Path & path, float display_scale) override {
      sendPath(path, display_scale);
      CGContextClip(gc);
    }
    void resetClip() override {
      // implement
    }
    void save() override {
      initializeContext();
      CGContextSaveGState(gc);
    }
    void restore() override {
      initializeContext();
      CGContextRestoreGState(gc);
    }

  protected:
    void sendPath(const Path & path, float display_scale);
    CGColorRef createCGColor(const Color & color, float globalAlpha = 1.0f) {
      CGFloat cv[] = { color.red, color.green, color.blue, color.alpha * globalAlpha};
      return CGColorCreate(cache->getColorSpace(), cv);
    }
    
    void setShadow(float shadowOffsetX, float shadowOffsetY, float shadowBlur, const Color & shadowColor, float display_scale) {
      initializeContext();
      CGSize offset = CGSizeMake(display_scale * shadowOffsetX, display_scale * shadowOffsetY);
      if (active_shadow_color) {
	if (CFGetRetainCount(active_shadow_color) != 1) std::cerr << "leaking memory Q!\n";
	CGColorRelease(active_shadow_color);
      }
      active_shadow_color = createCGColor(shadowColor);
      CGContextSetShadowWithColor(gc, offset, display_scale * shadowBlur, active_shadow_color);
    }
    
    void initializeContext() {
      if (!gc) {
        assert(bitmapData);
        unsigned int bitmapBytesPerRow = getActualWidth() * 4;
        gc = CGBitmapContextCreate(bitmapData, getActualWidth(), getActualHeight(), 8, bitmapBytesPerRow, cache->getColorSpace(),
                                   (getFormat() == RGBA8 ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast)); // | kCGBitmapByteOrder32Big);
        CGContextSetInterpolationQuality(gc, kCGInterpolationHigh);
        CGContextSetShouldAntialias(gc, true);
	flipY();
      }
    }

    void flipY() {
      CGContextTranslateCTM(gc, 0, getActualHeight());
      CGContextScaleCTM(gc, 1.0, -1.0);
    }
    
  private:
    Quartz2DCache * cache;
    CGContextRef gc = 0;
    unsigned char * bitmapData = 0;
    CGColorRef active_shadow_color = 0;
  };

  class ContextQuartz2D : public Context {
  public:
  ContextQuartz2D(Quartz2DCache * _cache, unsigned int _width, unsigned int _height, const InternalFormat & format, float _display_scale)
    : Context(_display_scale),
      cache(_cache),
      default_surface(_cache, _width, _height, (unsigned int)(_width * _display_scale), (unsigned int)(_height * _display_scale), format)
      {
      }

    std::shared_ptr<Surface> createSurface(const Image & image) override {
        return std::shared_ptr<Surface>(new Quartz2DSurface(cache, image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const InternalFormat & _format) override {
      return std::shared_ptr<Surface>(new Quartz2DSurface(cache, _width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), _format));
    }
      std::shared_ptr<Surface> createSurface(const std::string & filename) override {
          return std::shared_ptr<Surface>(new Quartz2DSurface(cache, filename));
      }
    // void clearRect(double x, double y, double w, double h) { }
        
    Surface & getDefaultSurface() override { return default_surface; }
    const Surface & getDefaultSurface() const override { return default_surface; }

#if 0      
    Context & drawImage(const Image & img, double x, double y, double w, double h) override;
    Context & drawImage(Surface & img, double x, double y, double w, double h) override;
#endif
    
  protected:
#if 0
    Context & renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op) override;
    Context & renderPath(RenderMode mode, const Path & path, const Style & style, Operator op) override;
#endif
    bool hasNativeShadows() const override { return true; }
    
  private:
    Quartz2DCache * cache;
    Quartz2DSurface default_surface;
  };

  class Quartz2DContextFactory : public ContextFactory {
  public:
    Quartz2DContextFactory(float _display_scale, std::shared_ptr<FilenameConverter> & _converter) : ContextFactory(_display_scale), converter(_converter) { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, const InternalFormat & format, bool apply_scaling) override {
      std::shared_ptr<Context> ptr(new ContextQuartz2D(&cache, width, height, format, apply_scaling ? getDisplayScale() : 1.0f));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      std::string filename2;
      if (converter->convert(filename, filename2)) {
        std::shared_ptr<Surface> ptr(new Quartz2DSurface(&cache, filename2));
        return ptr;
      } else {
        return createSurface(16, 16, RGBA8, false);
      }
    }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, const InternalFormat & format, bool apply_scaling) override {
      unsigned int aw = apply_scaling ? width * getDisplayScale() : width;
      unsigned int ah = apply_scaling ? height * getDisplayScale() : height;
      std::shared_ptr<Surface> ptr(new Quartz2DSurface(&cache, width, height, aw, ah, format));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) override {
      std::shared_ptr<Surface> ptr(new Quartz2DSurface(&cache, buffer, size));
      return ptr;
    }
  private:
    Quartz2DCache cache;
    std::shared_ptr<FilenameConverter> converter;
  };
};

#endif
