#include "ContextCairo.h"

#include <cassert>
#include <cmath>

using namespace canvas;

#if 0
Mutex CairoMapCanvas::pango_mutex;
Mutex CairoMapCanvas::draw_mutex;
#endif

using namespace std;

CairoSurface::CairoSurface(unsigned int _width, unsigned int _height)
  : Surface(_width, _height) {
  cairo_format_t format = CAIRO_FORMAT_ARGB32;
  // format = CAIRO_FORMAT_RGB24;
  surface = cairo_image_surface_create(format, _width, _height);
  assert(surface);
  cr = cairo_create(surface);  
  assert(cr);
}
  
CairoSurface::CairoSurface(unsigned int _width, unsigned int _height, const unsigned char * data)
  : Surface(_width, _height)
{
  cairo_format_t format = CAIRO_FORMAT_RGB24;
  unsigned int stride = cairo_format_stride_for_width(format, _width);
  assert(stride == 4 * _width);
  storage = new unsigned int[_width * _height];
  for (unsigned int i = 0; i < _width * _height; i++) {
    storage[i] = data[3 * i + 2] + (data[3 * i + 1] << 8) + (data[3 * i + 0] << 16);
  }
  surface = cairo_image_surface_create_for_data((unsigned char*)storage,
						format,
						_width,
						_height,
						stride);
  assert(surface);
  cr = cairo_create(surface);  
  assert(cr);
}

CairoSurface::CairoSurface(const std::string & filename) : Surface(0, 0)
{
  surface = cairo_image_surface_create_from_png(filename.c_str());
  assert(surface);
  Surface::resize(cairo_image_surface_get_width(surface),
		  cairo_image_surface_get_height(surface));
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
CairoSurface::resize(unsigned int _width, unsigned int _height) {
  Surface::resize(_width, _height);
  if (cr) cairo_destroy(cr);
  if (surface) cairo_surface_destroy(surface);  
  cairo_format_t format = CAIRO_FORMAT_ARGB32;
  // format = CAIRO_FORMAT_RGB24;
  surface = cairo_image_surface_create(format, _width, _height);
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
CairoSurface::stroke(const Path & path, const Style & style, double lineWidth) {
  sendPath(path);
  cairo_set_line_width(cr, lineWidth);
  // cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_source_rgba(cr, style.color.red, style.color.green, style.color.blue, style.color.alpha);
  cairo_stroke(cr);  
}

void
CairoSurface::fill(const Path & path, const Style & style) {
  sendPath(path);
  if (style.getType() == Style::LINEAR_GRADIENT) {
    cairo_pattern_t * pat = cairo_pattern_create_linear(style.x0, style.y0, style.x1, style.y1);
    for (map<float, Color>::const_iterator it = style.getColors().begin(); it != style.getColors().end(); it++) {
      cairo_pattern_add_color_stop_rgba(pat, it->first, it->second.red, it->second.green, it->second.blue, it->second.alpha);
    }
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  } else {
    cairo_set_source_rgba(cr, style.color.red, style.color.green, style.color.blue, style.color.alpha);
    cairo_fill(cr);
  }
}

#if 0
unsigned char *
CairoSurface::getBuffer() {
  assert(surface);
  return cairo_image_surface_get_data(surface);
}
#endif

void
CairoSurface::prepareTextStyle(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign) {
  cairo_set_source_rgba(cr, style.color.red, style.color.green, style.color.blue, style.color.alpha);
  cairo_select_font_face(cr, font.family.c_str(),
			 font.slant == Font::NORMAL_SLANT ? CAIRO_FONT_SLANT_NORMAL : (font.slant == Font::ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_OBLIQUE),
			 font.weight == Font::NORMAL || font.weight == Font::LIGHTER ? CAIRO_FONT_WEIGHT_NORMAL : CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, font.size);
}

void
CairoSurface::fillText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) {
  prepareTextStyle(font, style, textBaseline, textAlign);

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

  switch (textAlign) {
  case LEFT: break;
  case CENTER: x -= text_extents.width / 2; break;
  case RIGHT: x -= text_extents.width; break;
  default: break;
  }
  
  cairo_move_to(cr, x + 0.5, y + 0.5);
  cairo_show_text(cr, text.c_str());
}

void
CairoSurface::strokeText(const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y) {
  prepareTextStyle(font, style, textBaseline, textAlign);

  cairo_text_extents_t extents;
  cairo_text_extents(cr, text.c_str(), &extents);
  
  switch (textBaseline.getType()) {
  case TextBaseline::MIDDLE: y -= (extents.height/2 + extents.y_bearing); break;
  case TextBaseline::TOP: y += extents.height; break;
  default: break;
  }

  switch (textAlign) {
  case LEFT: break;
  case CENTER: x -= extents.width / 2; break;
  case RIGHT: x -= extents.width; break;
  default: break;
  }
  
  cairo_move_to(cr, x + 0.5, y + 0.5);
  cairo_text_path(cr, text.c_str());
  cairo_stroke(cr);
}  

void
CairoSurface::drawImage(Surface & _img, double x, double y, double w, double h) {
  CairoSurface & img = dynamic_cast<CairoSurface&>(_img);
  double sx = w / img.getWidth(), sy = h / img.getHeight();
  cairo_save(cr);
  cairo_scale(cr, sx, sy);
  cairo_set_source_surface(cr, img.surface, (x / sx) + 0.5, (y / sy) + 0.5);
  cairo_paint(cr);
  cairo_set_source_rgb(cr, 0.0f, 0.0f, 0.0f); // is this needed?
  cairo_restore(cr);
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
  
  // double pxscale = preport->hPX>preport->vPX ? preport->hPX:preport->vPX;
  // cairo_text_extents_t te;

  // cairo_scale(*ppcr, pxscale, pxscale);

  // cairo_set_font_size(cr, FONTSIZENORMAL);              // font
  // cairo_select_font_face(cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

  // cairo_text_extents(*ppcr, preport->ptitle, &te);       // center title
  // preport->titleH = 0.5 - te.x_bearing - te.width / 2;

  // cairo_set_line_width(*ppcr, 0.001*preport->linewidth);  // frame
  // preport->legendlinewidth = 0.003/powf((preport->hPX*preport->vPX)/1E4, 0.3)

#if 0
  // MutexLocker mh(pango_mutex);
  font_description = pango_font_description_new();
  pango_font_description_set_family(font_description, "sans-serif");
  // pango_font_description_set_weight(font_description, PANGO_WEIGHT_BOLD);
  pango_font_description_set_absolute_size(font_description, 11 * PANGO_SCALE);
  // mh.release();
#endif
}

ContextCairo::~ContextCairo() {
#if 0
  if (font_description) {
    // MutexLocker mh(pango_mutex);
    pango_font_description_free(font_description);
  }
#endif
}

TextMetrics
ContextCairo::measureText(const std::string & text) {
  cairo_select_font_face(default_surface.cr, font.family.c_str(),
			 font.slant == Font::NORMAL_SLANT ? CAIRO_FONT_SLANT_NORMAL : (font.slant == Font::ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_OBLIQUE),
			 font.weight == Font::NORMAL || font.weight == Font::LIGHTER ? CAIRO_FONT_WEIGHT_NORMAL : CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(default_surface.cr, font.size);
  cairo_text_extents_t te;
  cairo_text_extents(default_surface.cr, text.c_str(), &te);
  return { (float)te.width, (float)te.height };
}

