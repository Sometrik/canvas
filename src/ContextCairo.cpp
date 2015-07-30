#include "ContextCairo.h"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace canvas;
using namespace std;

CairoSurface::CairoSurface(unsigned int _width, unsigned int _height, bool has_alpha)
  : Surface(_width, _height, _width, _height) {
  cairo_format_t format = has_alpha ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;
  surface = cairo_image_surface_create(format, _width, _height);
  assert(surface);
  cr = cairo_create(surface);  
  assert(cr);
}
 
CairoSurface::CairoSurface(const Image & image)
  : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight())
{
  cairo_format_t format = image.hasAlpha() ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;
  unsigned int stride = cairo_format_stride_for_width(format, getActualWidth());
  assert(stride == 4 * getActualWidth());
  storage = new unsigned int[getActualWidth() * getActualHeight()];
  const unsigned char * data = image.getData();
  if (image.hasAlpha()) {
    for (unsigned int i = 0; i < getActualWidth() * getActualHeight(); i++) {
#if 0
      storage[i] = (data[4 * i + 0] << 24) + (data[4 * i + 1] << 16) + (data[4 * i + 2] << 8) + data[4 * i + 3];
#else
      storage[i] = (data[4 * i + 0]) + (data[4 * i + 1] << 8) + (data[4 * i + 2] << 16) + (data[4 * i + 3] << 24);
#endif
    }
  } else {
    for (unsigned int i = 0; i < getActualWidth() * getActualHeight(); i++) {
      storage[i] = data[3 * i + 2] + (data[3 * i + 1] << 8) + (data[3 * i + 0] << 16);
    }
  }
  surface = cairo_image_surface_create_for_data((unsigned char*)storage,
						format,
						getActualWidth(),
						getActualHeight(),
						stride);
  assert(surface);
  cr = cairo_create(surface);  
  assert(cr);
}

CairoSurface::CairoSurface(const std::string & filename) : Surface(0, 0, 0, 0) {
  surface = cairo_image_surface_create_from_png(filename.c_str());
  assert(surface);
  unsigned int w = cairo_image_surface_get_width(surface), h = cairo_image_surface_get_height(surface);
  Surface::resize(w, h, w, h);		  
  cr = cairo_create(surface);
  assert(cr);
}

struct read_buffer_s {
  size_t offset, size;
  const unsigned char * data;
};

static cairo_status_t read_buffer(void *closure, unsigned char *data, unsigned int length)
{
  read_buffer_s * buf = (read_buffer_s*)closure;

  if (buf->offset + length > buf->size) {
    return CAIRO_STATUS_READ_ERROR;
  }
  memcpy(data, buf->data + buf->offset, length);
  buf->offset += length;
  return CAIRO_STATUS_SUCCESS;
}

CairoSurface::CairoSurface(const unsigned char * buffer, size_t size) : Surface(16, 16, 16, 16) {
  read_buffer_s buf = { 0, size, buffer };
  if (isPNG(buffer, size)) {
    surface = cairo_image_surface_create_from_png_stream(read_buffer, &buf);
    unsigned int w = cairo_image_surface_get_width(surface), h = cairo_image_surface_get_height(surface);
    Surface::resize(w, h, w, h);
  } else {
    cerr << "failed to load image from memory\n";
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, getActualWidth(), getActualHeight());
  }
  assert(surface);
  cr = cairo_create(surface);
  assert(cr);
}

CairoSurface::~CairoSurface() {
  if (cr) {
    cairo_destroy(cr);
  }
  if (surface) {
    cairo_surface_destroy(surface);
  }
  delete[] storage;
} 

void
CairoSurface::flush() {
  cairo_surface_flush(surface);
}

void
CairoSurface::markDirty() {
  cairo_surface_mark_dirty(surface);
}

void
CairoSurface::resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height) {
  Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height);
  if (cr) cairo_destroy(cr);
  if (surface) cairo_surface_destroy(surface);  
  cairo_format_t format = CAIRO_FORMAT_ARGB32;
  // format = CAIRO_FORMAT_RGB24;
  surface = cairo_image_surface_create(format, _actual_width, _actual_height);
  assert(surface);
  cr = cairo_create(surface);
  assert(cr);
} 

void
CairoSurface::sendPath(const Path & path) {
  for (auto pc : path.getData()) {
    switch (pc.type) {
    case PathComponent::MOVE_TO: cairo_move_to(cr, pc.x0 + 0.5, pc.y0 + 0.5); break;
    case PathComponent::LINE_TO: cairo_line_to(cr, pc.x0 + 0.5, pc.y0 + 0.5); break;
    case PathComponent::CLOSE: cairo_close_path(cr); break;
    case PathComponent::ARC:
      if (!pc.anticlockwise) {
	cairo_arc(cr, pc.x0 + 0.5, pc.y0 + 0.5, pc.radius, pc.sa, pc.ea);
      } else {
	cairo_arc_negative(cr, pc.x0 + 0.5, pc.y0 + 0.5, pc.radius, pc.sa, pc.ea);
      }
      break;
    }
  }
}

void
CairoSurface::clip(const Path & path) {
  sendPath(path);
  cairo_clip(cr);
}

void
CairoSurface::renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth) {
  cairo_pattern_t * pat = 0;
  if (style.getType() == Style::LINEAR_GRADIENT) {
    pat = cairo_pattern_create_linear(style.x0, style.y0, style.x1, style.y1);
    for (map<float, Color>::const_iterator it = style.getColors().begin(); it != style.getColors().end(); it++) {
      cairo_pattern_add_color_stop_rgba(pat, it->first, it->second.red, it->second.green, it->second.blue, it->second.alpha);
    }
    cairo_set_source(cr, pat);    
  } else if (style.getType() == Style::FILTER) {
    double min_x, min_y, max_x, max_y;
    path.getExtents(min_x, min_y, max_x, max_y);
  } else {
    cairo_set_source_rgba(cr, style.color.red, style.color.green, style.color.blue, style.color.alpha);
  }
  sendPath(path);
  switch (mode) {
  case STROKE:
    cairo_set_line_width(cr, lineWidth);
    // cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr);  
    break;
  case FILL:
    cairo_fill(cr);
    break;
  }
  
  if (pat) {
    cairo_pattern_destroy(pat);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  }
}

void
CairoSurface::renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, float display_scale) {
  cairo_set_source_rgba(cr, style.color.red, style.color.green, style.color.blue, style.color.alpha);
  cairo_select_font_face(cr, font.family.c_str(),
			 font.slant == Font::NORMAL_SLANT ? CAIRO_FONT_SLANT_NORMAL : (font.slant == Font::ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_OBLIQUE),
			 font.weight == Font::NORMAL || font.weight == Font::LIGHTER ? CAIRO_FONT_WEIGHT_NORMAL : CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, font.size * display_scale);

  cairo_font_extents_t font_extents;
  cairo_font_extents(cr, &font_extents);
  
  cairo_text_extents_t text_extents;
  cairo_text_extents(cr, text.c_str(), &text_extents);
  
  switch (textBaseline.getType()) {
    // case TextBaseline::MIDDLE: y -= (extents.height/2 + extents.y_bearing); break;
  case TextBaseline::MIDDLE: y += -font_extents.descent + (font_extents.ascent + font_extents.descent) / 2.0; break;
  case TextBaseline::TOP: y += font_extents.ascent; break;
  default: break;
  }

  switch (textAlign.getType()) {
  case TextAlign::LEFT: break;
  case TextAlign::CENTER: x -= text_extents.width / 2; break;
  case TextAlign::RIGHT: x -= text_extents.width; break;
  default: break;
  }
  
  cairo_move_to(cr, x * display_scale + 0.5, y * display_scale + 0.5);
  
  switch (mode) {
  case STROKE:
    cairo_set_line_width(cr, lineWidth);
    cairo_text_path(cr, text.c_str());
    cairo_stroke(cr);
    break;
  case FILL:
    cairo_show_text(cr, text.c_str());
    break;
  }
}

void
CairoSurface::drawNativeSurface(CairoSurface & img, double x, double y, double w, double h, float alpha, bool imageSmoothingEnabled) {
  double sx = w / img.getActualWidth(), sy = h / img.getActualHeight();
  cairo_save(cr);
  cairo_scale(cr, sx, sy);
  cairo_set_source_surface(cr, img.surface, (x / sx) + 0.5, (y / sy) + 0.5);
  cairo_pattern_set_filter (cairo_get_source (cr), imageSmoothingEnabled ? CAIRO_FILTER_BILINEAR : CAIRO_FILTER_NEAREST);
  if (alpha < 1.0f) {
    cairo_paint_with_alpha(cr, alpha);
  } else {
    cairo_paint(cr);
  }
  cairo_set_source_rgb(cr, 0.0f, 0.0f, 0.0f); // is this needed?
  cairo_restore(cr);
}

void
CairoSurface::drawImage(Surface & _img, double x, double y, double w, double h, float alpha, bool imageSmoothingEnabled) {
  CairoSurface * cs = dynamic_cast<CairoSurface*>(&_img);
  if (cs) {
    drawNativeSurface(*cs, x, y, w, h, alpha, imageSmoothingEnabled);    
  } else {
    auto img = _img.createImage();
    CairoSurface cs(*img);
    drawNativeSurface(cs, x, y, w, h, alpha, imageSmoothingEnabled);
  }
}

void
CairoSurface::save() {  
  cairo_save(cr);
}

void
CairoSurface::restore() {
  cairo_restore(cr);
}

ContextCairo::ContextCairo(unsigned int _width, unsigned int _height)
  : Context(_width, _height),
    default_surface(_width, _height)
{ 
}

TextMetrics
ContextCairo::measureText(const std::string & text) {
  cairo_select_font_face(default_surface.cr, font.family.c_str(),
			 font.slant == Font::NORMAL_SLANT ? CAIRO_FONT_SLANT_NORMAL : (font.slant == Font::ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_OBLIQUE),
			 font.weight == Font::NORMAL || font.weight == Font::LIGHTER ? CAIRO_FONT_WEIGHT_NORMAL : CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(default_surface.cr, font.size);
  cairo_text_extents_t te;
  cairo_text_extents(default_surface.cr, text.c_str(), &te);
  return TextMetrics((float)te.width, (float)te.height);
}

