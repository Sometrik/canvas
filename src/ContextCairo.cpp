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

#if 0
unsigned char *
CairoSurface::getBuffer() {
  assert(surface);
  return cairo_image_surface_get_data(surface);
}
#endif

void
CairoSurface::fillText(Context & context, const std::string & text, double x, double y) {
  cairo_set_source_rgba(cr, context.fillStyle.color.red / 255.0f, context.fillStyle.color.green / 255.0f, context.fillStyle.color.blue / 255.0f, 1.0f);
  cairo_select_font_face(cr, context.font.family.c_str(),
			 context.font.slant == Font::NORMAL_SLANT ? CAIRO_FONT_SLANT_NORMAL : (context.font.slant == Font::ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_OBLIQUE),
			 context.font.weight == Font::NORMAL || context.font.weight == Font::LIGHTER ? CAIRO_FONT_WEIGHT_NORMAL : CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, context.font.size);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, text.c_str(), &extents);

  switch (context.textBaseline.getType()) {
  case TextBaseline::MIDDLE: y -= (extents.height/2 + extents.y_bearing); break;
  case TextBaseline::TOP: y += extents.height; break;
  default: break;
  }

  switch (context.textAlign) {
  case LEFT: break;
  case CENTER: x -= extents.width / 2; break;
  case RIGHT: x -= extents.width; break;
  default: break;
  }
  
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, text.c_str());
}

void
CairoSurface::drawImage(Surface & _img, double x, double y, double w, double h) {
  CairoSurface & img = dynamic_cast<CairoSurface&>(_img);
  cairo_save(cr);
  cairo_scale(cr, w / img.getWidth(), h / img.getHeight());
  cairo_set_source_surface(cr, img.surface, x, y);
  cairo_paint(cr);
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

void
ContextCairo::resize(unsigned int _width, unsigned int _height) {
  Context::resize(_width, _height);
}

void
ContextCairo::save() {
  cairo_save(default_surface.cr);
}

void
ContextCairo::restore() {
  cairo_restore(default_surface.cr);
}

void
ContextCairo::beginPath() {
  cairo_new_path(default_surface.cr);
}

void
ContextCairo::closePath() {
  cairo_close_path(default_surface.cr);
}

void
ContextCairo::clip() {
  cairo_clip(default_surface.cr);
}

void
ContextCairo::arc(double x, double y, double r, double sa, double ea, bool anticlockwise) {
  double span = 0;

  if ((!anticlockwise && (ea - sa >= 2 * M_PI)) || (anticlockwise && (sa - ea >= 2 * M_PI))) {
    // If the anticlockwise argument is false and endAngle-startAngle is equal to or greater than 2*PI, or, if the
    // anticlockwise argument is true and startAngle-endAngle is equal to or greater than 2*PI, then the arc is the whole
    // circumference of this circle.
    span = 2 * M_PI;
  } else {
    if (!anticlockwise && (ea < sa)) {
      span += 2 * M_PI;
    } else if (anticlockwise && (sa < ea)) {
      span -= 2 * M_PI;
    }
 
#if 0
    // this is also due to switched coordinate system
    // we would end up with a 0 span instead of 360
    if (!(qFuzzyCompare(span + (ea - sa) + 1, 1.0) && qFuzzyCompare(qAbs(span), 360.0))) {
      // mod 360
      span += (ea - sa) - (static_cast<int>((ea - sa) / 360)) * 360;
    }
#else
    span += ea - sa;
#endif
  }
 
#if 0
  // If the path is empty, move to where the arc will start to avoid painting a line from (0,0)
  // NOTE: QPainterPath::isEmpty() won't work here since it ignores a lone MoveToElement
  if (!m_path.elementCount())
    m_path.arcMoveTo(xs, ys, width, height, sa);
  else if (!radius) {
    m_path.lineTo(xc, yc);
    return;
  }
#endif

  if (!anticlockwise) {
    cairo_arc(default_surface.cr, x, y, r, sa, sa + span);
  } else {
    cairo_arc_negative(default_surface.cr, x, y, r, sa, sa + span);
  }
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

void
ContextCairo::moveTo(double x, double y) {
  cairo_move_to(default_surface.cr, x, y);
}

void
ContextCairo::lineTo(double x, double y) {
  cairo_line_to(default_surface.cr, x, y);    
}

void
ContextCairo::stroke() {
  cairo_set_line_width(default_surface.cr, lineWidth);
  // cairo_set_line_join(default_surface.cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_source_rgba(default_surface.cr, strokeStyle.color.red / 255.0f, strokeStyle.color.green / 255.0f, strokeStyle.color.blue / 255.0f, 1.0f);
  cairo_stroke_preserve(default_surface.cr);
}

void
ContextCairo::fill() {
  if (fillStyle.getType() == Style::LINEAR_GRADIENT) {
    cairo_pattern_t * pat = cairo_pattern_create_linear(fillStyle.x0, fillStyle.y0, fillStyle.x1, fillStyle.y1);
    for (map<float, Color>::const_iterator it = fillStyle.getColors().begin(); it != fillStyle.getColors().end(); it++) {
      cairo_pattern_add_color_stop_rgba(pat, it->first, it->second.red / 255.0f, it->second.green / 255.0f, it->second.blue / 255.0f, 1);
    }
    cairo_set_source (default_surface.cr, pat);
    cairo_pattern_destroy (pat);
  } else {
    cairo_set_source_rgba(default_surface.cr, fillStyle.color.red / 255.0f, fillStyle.color.green / 255.0f, fillStyle.color.blue / 255.0f, 1.0f);
  }
  cairo_fill_preserve(default_surface.cr);
}

Point
ContextCairo::getCurrentPoint() {
  double x0, y0;
  cairo_get_current_point(default_surface.cr, &x0, &y0);
  return Point(x0, y0);
}
