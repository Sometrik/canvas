#ifndef _CANVAS_CONTEXT_H_
#define _CANVAS_CONTEXT_H_

#include <GraphicsState.h>

#include <Color.h>
#include <Surface.h>
#include <Image.h>
#include <HitRegion.h>

#include <string>
#include <memory>

namespace canvas {
  class Context : public GraphicsState {
  public:
    Context(float _display_scale = 1.0f)
      : display_scale(_display_scale),
      current_linear_gradient(this)
      { }
    Context(const Context & other) = delete;
    Context & operator=(const Context & other) = delete;
    Context & operator=(const GraphicsState & other) {
      GraphicsState::operator=(other);
      return *this;
    }
    virtual ~Context() = default;

    virtual std::unique_ptr<Surface> createSurface(const ImageData & image) = 0;
    virtual std::unique_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, unsigned int _num_channels) = 0;
    virtual Surface & getDefaultSurface() = 0;
    virtual const Surface & getDefaultSurface() const = 0;

    virtual bool hasNativeShadows() const { return false; }
    virtual bool hasNativeEmoticons() const { return false; }

    virtual void resize(unsigned int _width, unsigned int _height) {
      getDefaultSurface().resize(_width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), getDefaultSurface().getNumChannels());
      hit_regions.clear();
    }
        
    Context & stroke() { return renderPath(RenderMode::STROKE, currentPath, strokeStyle); }
    Context & stroke(const Path2D & path) { return renderPath(RenderMode::STROKE, path, strokeStyle); }
    Context & fill() { return renderPath(RenderMode::FILL, currentPath, fillStyle); }
    Context & fill(const Path2D & path) { return renderPath(RenderMode::FILL, path, fillStyle); }
    Context & save() {
      restore_stack.push_back(*this);
      return *this;
    }
    Context & restore() {
      if (!restore_stack.empty()) {
	*this = restore_stack.back();
	restore_stack.pop_back();    
      }
      return *this;
    }
    
    bool isPointInPath(const Path2D & path, double x, double y) { return false; }
    
    TextMetrics measureText(const std::string & text) {
      return getDefaultSurface().measureText(font, text, textBaseline.get(), getDisplayScale());
    }
    
    Context & fillRect(double x, double y, double w, double h) {
      Path2D path;
      path.rect(x, y, w, h);
      return fill(path);
    }
    
    Context & strokeRect(double x, double y, double w, double h) {
      Path2D path;
      path.rect(x, y, w, h);
      return stroke(path);
    }
    
    Context & clearRect(double x, double y, double w, double h) {
      Path2D path;
      path.rect(x, y, w, h);
      Style style(this);
      style = Color(0.0f, 0.0f, 0.0f, 0.0f);
      return renderPath(RenderMode::FILL, path, style, Operator::COPY);
    }
    
    Context & fillText(const std::string & text, double x, double y) { return renderText(RenderMode::FILL, fillStyle, text, Point(x, y)); }
    Context & strokeText(const std::string & text, double x, double y) { return renderText(RenderMode::STROKE, strokeStyle, text, Point(x, y)); }
    
    unsigned int getWidth() const { return getDefaultSurface().getLogicalWidth(); }
    unsigned int getHeight() const { return getDefaultSurface().getLogicalHeight(); }
    unsigned int getActualWidth() const { return getDefaultSurface().getActualWidth(); }
    unsigned int getActualHeight() const { return getDefaultSurface().getActualHeight(); }

    Context & drawImage(Context & other, double x, double y, double w, double h) {
      return drawImage(other.getDefaultSurface(), x, y, w, h);
    }
    
    Context & drawImage(Image & img, double x, double y, double w, double h) {
      return drawImage(img.getData(), x, y, w, h);
    }
    
    virtual Context & drawImage(const ImageData & img, double x, double y, double w, double h) {
      auto p = currentTransform.multiply(x, y);
      if (hasNativeShadows()) {
	getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath, imageSmoothingEnabled.get());
      } else {
	if (hasShadow()) {
	  float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
	  float bi = int(ceil(b));
	  auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, 1);
	  shadow->drawImage(img, Point(x + b + shadowOffsetX.get(), y + b + shadowOffsetY.get()), w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
	  // shadow->colorFill(shadowColor.get());
	  auto shadow1 = shadow->blur(bs, bs);
	  auto shadow2 = shadow1->colorize(shadowColor.get());
	  getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
	}
	getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
      }
      return *this;
    }
    
    virtual Context & drawImage(Surface & img, double x, double y, double w, double h) {
      auto p = currentTransform.multiply(x, y);
      if (hasNativeShadows()) {
	getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath, imageSmoothingEnabled.get());
      } else {
	if (hasShadow()) {
	  float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
	  float bi = int(ceil(b));
	  auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, 1);
	  
	  shadow->drawImage(img, Point(p.x + b + shadowOffsetX.get(), p.y + b + shadowOffsetY.get()), w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
	  // shadow->colorFill(shadowColor.get());
	  auto shadow1 = shadow->blur(bs, bs);
	  auto shadow2 = shadow1->colorize(shadowColor.get());
	  getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
	}
	getDefaultSurface().drawImage(img, p, w, h, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath, imageSmoothingEnabled.get());
      }
      return *this;
    }
        
    Style & createLinearGradient(double x0, double y0, double x1, double y1) {
      current_linear_gradient.setType(Style::LINEAR_GRADIENT);
      current_linear_gradient.setVector(x0, y0, x1, y1);
      return current_linear_gradient;
    }

    float getDisplayScale() const { return display_scale; }
    Context & addHitRegion(const std::string & id, const std::string & cursor) {
      if (!currentPath.empty()) {
	HitRegion hr(id, currentPath, cursor);
	hit_regions.push_back(hr);
      }
      return *this;
    }
#if 0
    const HitRegion & getHitRegion(float x, float y) const {
      for (auto & r : hit_regions) {
	if (r.isInside(x, y)) return r;
      }
      return null_region;
    }
#endif
    const std::vector<HitRegion> & getHitRegions() const { return hit_regions; }
    
#if 0
    Style & createPattern(const ImageData & image, const char * repeat) {
      
    }
    Style & createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1) {
     }
#endif
    
  protected:
    Context & renderPath(RenderMode mode, const Path2D & path0, const Style & style0, Operator op = Operator::SOURCE_OVER) {
      auto scaled_lineWidth = currentTransform.transformSize(lineWidth.get());
      if (mode != RenderMode::STROKE || scaled_lineWidth > 0.1f) {
	auto path = path0.transform(currentTransform);
	auto style = style0.transform(currentTransform);
	
	if (hasNativeShadows()) {
	  getDefaultSurface().renderPath(mode, path, style, scaled_lineWidth, op, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath);
	} else {
	  if (hasShadow()) {
	    float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
	    float bi = int(ceil(b));
#ifdef WIN32
	    auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, 4);
	    Style shadow_style(this);
	    shadow_style = shadowColor.get();
	    Path2D tmp_path = path, tmp_clipPath = clipPath;
	    tmp_path.offset(shadowOffsetX.get() + bi, shadowOffsetY.get() + bi);
	    tmp_clipPath.offset(shadowOffsetX.get() + bi, shadowOffsetY.get() + bi);
	    
	    shadow->renderPath(mode, tmp_path, shadow_style, scaled_lineWidth, op, getDisplayScale(), globalAlpha.get(), 0, 0, 0, shadowColor.get(), tmp_clipPath);
	    auto shadow1 = shadow->blur(bs, bs);
	    getDefaultSurface().drawImage(*shadow1, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
#else
	    auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, 1);
	    Style shadow_style(this);
	    shadow_style = shadowColor.get();
	    Path2D tmp_path = path, tmp_clipPath = clipPath;
	    tmp_path.offset(shadowOffsetX.get() + bi, shadowOffsetY.get() + bi);
	    tmp_clipPath.offset(shadowOffsetX.get() + bi, shadowOffsetY.get() + bi);
	    
	    shadow->renderPath(mode, tmp_path, shadow_style, scaled_lineWidth, op, getDisplayScale(), globalAlpha.get(), 0, 0, 0, shadowColor.get(), tmp_clipPath);
	    auto shadow1 = shadow->blur(bs, bs);
	    auto shadow2 = shadow1->colorize(shadowColor.get());
	    getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
#endif
	  }
	  getDefaultSurface().renderPath(mode, path, style, scaled_lineWidth, op, getDisplayScale(), globalAlpha.get(), 0, 0, 0, shadowColor.get(), clipPath);
	}
      }
      return *this;
    }
    
    Context & renderText(RenderMode mode, const Style & style0, const std::string & text, const Point & p0, Operator op = Operator::SOURCE_OVER) {
      auto scaled_font_size = currentTransform.transformSize(font.size);
      auto scaled_lineWidth = currentTransform.transformSize(lineWidth.get());
      if (scaled_font_size > 0.5f && (mode != RenderMode::STROKE || scaled_lineWidth > 0.1f)) {
	auto p = currentTransform.multiply(p0);
	auto tmp_font = font;
	tmp_font.size = scaled_font_size;

	auto style = style0.transform(currentTransform);

	if (hasNativeShadows()) {
	  getDefaultSurface().renderText(mode, tmp_font, style, textBaseline.get(), textAlign.get(), text, p, scaled_lineWidth, op, getDisplayScale(), globalAlpha.get(), shadowBlur.get(), shadowOffsetX.get(), shadowOffsetY.get(), shadowColor.get(), clipPath);
	} else {
	  if (hasShadow()) {
	    float b = shadowBlur.get(), bs = shadowBlur.get() * getDisplayScale();
	    int bi = int(ceil(b));
	    auto shadow = createSurface(getDefaultSurface().getLogicalWidth() + 2 * bi, getDefaultSurface().getLogicalHeight() + 2 * bi, 1);
	  
	    Style shadow_style(this);
	    shadow_style = shadowColor.get();
	    shadow_style.color.alpha = 1.0f;
	    shadow->renderText(mode, tmp_font, shadow_style, textBaseline.get(), textAlign.get(), text, Point(p.x + shadowOffsetX.get() + b, p.y + shadowOffsetY.get() + b), scaled_lineWidth, op, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath);
	    auto shadow1 = shadow->blur(bs, bs);
	    auto shadow2 = shadow1->colorize(shadowColor.get());
	    getDefaultSurface().drawImage(*shadow2, Point(-b, -b), shadow->getLogicalWidth(), shadow->getLogicalHeight(), getDisplayScale(), 1.0f, 0.0f, 0.0f, 0.0f, shadowColor.get(), Path2D(), false);
	  }
	  getDefaultSurface().renderText(mode, tmp_font, style, textBaseline.get(), textAlign.get(), text, p, scaled_lineWidth, op, getDisplayScale(), globalAlpha.get(), 0.0f, 0.0f, 0.0f, shadowColor.get(), clipPath);
	}
      }
      return *this;
    }

    bool hasShadow() const { return shadowBlur.get() > 0.0f || shadowOffsetX.get() != 0 || shadowOffsetY.get() != 0; }
    
  private:
    float display_scale;
    Style current_linear_gradient;
    std::vector<GraphicsState> restore_stack;
    std::vector<HitRegion> hit_regions;
    HitRegion null_region;
  };
    
  class ContextFactory {
  public:
    ContextFactory(float _display_scale) : display_scale(_display_scale) { }
    virtual ~ContextFactory() { }
    virtual std::unique_ptr<Context> createContext(unsigned int width, unsigned int height, unsigned int num_channels = 4) = 0;
    virtual std::unique_ptr<Surface> createSurface(unsigned int width, unsigned int height, unsigned int num_channels = 4) = 0;
    virtual std::unique_ptr<Image> loadImage(const std::string & filename) = 0;
    virtual std::unique_ptr<Image> createImage() = 0;
    virtual std::unique_ptr<Image> createImage(const unsigned char * _data, unsigned int _width, unsigned int _height, unsigned int _num_channels) = 0;
    
    float getDisplayScale() const { return display_scale; }
    
  private:
    float display_scale;
  };

  class NullContext : public Context {
  public:
    NullContext() { }
  };
};

#endif
