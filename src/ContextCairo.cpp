#include "ContextCairo.h"

#include <cassert>

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
  
CairoSurface::CairoSurface(unsigned int _width, unsigned int _height, unsigned char * data)
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
  cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, context.font.size);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, text.c_str(), &extents);

  switch (context.textBaseline) {
  case MIDDLE:
    cairo_move_to(cr, x, y - (extents.height/2 + extents.y_bearing));
    break;
  case TOP:
    cairo_move_to(cr, x, y + extents.height / 2);
    break;
  default:
    cairo_move_to(cr, x, y);
  };
  
  cairo_show_text(cr, text.c_str());
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
  cairo_new_path(default_surface.cr); // current path is not consumed with cairo_clip
}

void
ContextCairo::arc(double x, double y, double r, double a0, double a1, bool t) {
  std::cerr << "drawing arc, this = " << this << ", cr = " << default_surface.cr << std::endl;
  if (!t) {
    cairo_arc(default_surface.cr, x, y, r, a0, a1);
  } else {
    cairo_arc_negative(default_surface.cr, x, y, r, a0, a1);
  }
}

Size
ContextCairo::measureText(const std::string & text) {
  cairo_select_font_face(default_surface.cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(default_surface.cr, 12);
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

void
ContextCairo::drawImage(Surface & _img, double x, double y, double w, double h) {
  CairoSurface & img = dynamic_cast<CairoSurface&>(_img);
  cairo_save(default_surface.cr);
  cairo_scale(default_surface.cr, w / img.getWidth(), h / img.getHeight());
  cairo_set_source_surface(default_surface.cr, img.surface, x, y);
  cairo_paint(default_surface.cr);
  cairo_restore(default_surface.cr);
}

// cairo_surface_mark_dirty
