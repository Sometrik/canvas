#include "ContextCairo.h"

#include <cassert>

using namespace canvas;

#if 0
Mutex CairoMapCanvas::pango_mutex;
Mutex CairoMapCanvas::draw_mutex;
#endif

using namespace std;

ContextCairo::ContextCairo(unsigned int width, unsigned int height)
  : Context(width, height), default_surface(width, height)
{
  cr = cairo_create(default_surface.surface);
  cerr << "created cairo context: " << cr << endl;
  
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
  cairo_destroy(cr);
#if 0
  if (font_description) {
    // MutexLocker mh(pango_mutex);
    pango_font_description_free(font_description);
  }
#endif
}

void
ContextCairo::resize(unsigned int width, unsigned int height) {
  Context::resize(width, height);

  cerr << "destroying old cairo\n";
  cairo_destroy(cr);
  cerr << "creating new surface\n";
  default_surface.resize(width, height);
  cerr << "creating new cairo\n";
  cr = cairo_create(default_surface.surface);
  cerr << "created new cairo context " << cr << "\n";
}

void
ContextCairo::check() const {
  cerr << "check, this = " << this << ", cr = " << cr << endl;
}

void
ContextCairo::save() {
  cairo_save(cr);
}

void
ContextCairo::restore() {
  cairo_restore(cr);
}

void
ContextCairo::clip() {
  cairo_clip(cr);
  cairo_new_path(cr); // current path is not consumed with cairo_clip
}

void
ContextCairo::arc(double x, double y, double r, double a0, double a1, bool t) {
  std::cerr << "drawing arc, this = " << this << ", cr = " << cr << std::endl;
  if (!t) {
    cairo_arc(cr, x, y, r, a0, a1);
  } else {
    cairo_arc_negative(cr, x, y, r, a0, a1);
  }
}

void
ContextCairo::fillText(const std::string & text, double x, double y) {
  cairo_set_source_rgba(cr, fillStyle.color.red / 255.0f, fillStyle.color.green / 255.0f, fillStyle.color.blue / 255.0f, 1.0f);
  cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 12);
  moveTo(x, y);
  cairo_show_text(cr, text.c_str());
}

Size
ContextCairo::measureText(const std::string & text) {
  std::cerr << "measuring text " << text << ", cr = " << cr << std::endl;
  cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  std::cerr << "step\n";
  cairo_set_font_size(cr, 12);
  std::cerr << "step2\n";
  cairo_text_extents_t te;
  std::cerr << "step3\n";
  cairo_text_extents(cr, text.c_str(), &te);
  std::cerr << "step4\n";
  return { (float)te.width, (float)te.height };
}

void
ContextCairo::moveTo(double x, double y) {
  cairo_move_to(cr, x, y);
}

void
ContextCairo::lineTo(double x, double y) {
  cairo_line_to(cr, x, y);    
}

void
ContextCairo::stroke() {
  // cairo_set_line_width(cr, thickness == 0 ? 1 : thickness);
  // cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_source_rgba(cr, strokeStyle.color.red / 255.0f, strokeStyle.color.green / 255.0f, strokeStyle.color.blue / 255.0f, 1.0f);
  cairo_stroke(cr);
}

void
ContextCairo::fill() {
  cairo_set_source_rgba(cr, fillStyle.color.red / 255.0f, fillStyle.color.green / 255.0f, fillStyle.color.blue / 255.0f, 1.0f);
  cairo_fill(cr);
}

void
ContextCairo::drawImage(Surface & _img, double x, double y, double w, double h) {
  CairoSurface & img = dynamic_cast<CairoSurface&>(_img);
  cairo_save(cr);
  cairo_scale(cr, w / img.getWidth(), h / img.getHeight());
  cairo_set_source_surface(cr, img.surface, 0, 0);
  cairo_paint(cr);
}
