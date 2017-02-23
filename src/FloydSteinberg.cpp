#include "FloydSteinberg.h"

#include <ImageData.h>

#include <cassert>

using namespace std;
using namespace canvas;
  
unique_ptr<ImageData>
FloydSteinberg::apply() {  
  auto target_size = width * height * 2;
  std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_size]);
  unsigned short * output_data = (unsigned short *)tmp.get();
  unsigned int n = target_size / 2;

  for (int y = 0; y < (int)height; y++) {
    for (int x = 0; x < (int)width; x++) {
      auto input_offset = 4 * (y * width + x);
      unsigned char old_r = input_data[input_offset++];
      unsigned char old_g = input_data[input_offset++];
      unsigned char old_b = input_data[input_offset++];
      unsigned char old_a = input_data[input_offset++];

      unsigned char r_error, g_error, b_error, a_error;

      if (target_format == RGBA4) {
	unsigned char r = old_r >> 4, g = old_g >> 4, b = old_b >> 4, a = old_a >> 4;
	r_error = old_r - (r << 4);
	g_error = old_g - (g << 4);
	b_error = old_b - (b << 4);
	a_error = old_a - (a << 4);
	
#ifdef __APPLE__
	*output_data++ = (r << 12) | (g << 8) | (b << 4) | a;
#else
	*output_data++ = (b << 12) | (g << 8) | (r << 4) | a;
#endif
      } else {	 
	unsigned char r = old_r >> 3, g = old_g >> 2, b = old_b >> 3;
	r_error = old_r - (r << 3);
	g_error = old_g - (g << 2);
	b_error = old_b - (b << 3);
	a_error = 0;

#ifdef __APPLE__
	*output_data++ = PACK_RGB565(b, g, r);
#else
	*output_data++ = PACK_RGB565(r, g, b);
#endif
      }

      addError(x + 1, y,     7.0f/16.0f, r_error, g_error, b_error, a_error);
      addError(x - 1, y + 1, 3.0f/16.0f, r_error, g_error, b_error, a_error);
      addError(x,     y + 1, 5.0f/16.0f, r_error, g_error, b_error, a_error);
      addError(x + 1, y + 1, 1.0f/16.0f, r_error, g_error, b_error, a_error);
    }
  }
  
  return unique_ptr<ImageData>(new ImageData(tmp.get(), target_format, width, height));
}
