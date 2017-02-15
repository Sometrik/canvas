#include "FloydSteinberg.h"

#include <cassert>

using namespace std;
using namespace canvas;

FloydSteinberg::FloydSteinberg(const ImageData * input_image, InternalFormat _target_format)
  : target_format(_target_format) {
  width = input_image->getWidth();
  height = input_image->getHeight();
  input_data = std::unique_ptr<unsigned char[]>(new unsigned char[width * height * 4]);
  auto tmp = input_data.get();

  auto data = input_image->getData();
  
  auto & fd = input_image->getImageFormat();
  unsigned int n = width * height;
  for (unsigned int i = 0; i < n; i++) {
    unsigned int input_offset = i * fd.getBytesPerPixel();
    unsigned char r = data[input_offset++];
    unsigned char g = fd.getBytesPerPixel() >= 1 ? data[input_offset++] : r;
    unsigned char b = fd.getBytesPerPixel() >= 2 ? data[input_offset++] : g;
    unsigned char a = fd.getBytesPerPixel() >= 3 ? data[input_offset++] : 0xff;
    *tmp++ = r;
    *tmp++ = g;
    *tmp++ = b;
    *tmp++ = a;
  }
}
  
unique_ptr<ImageData>
FloydSteinberg::apply() {  
  auto target_size = width * height * 2;
  std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_size]);
  unsigned short * output_data = (unsigned short *)tmp.get();
  unsigned int n = target_size / 2;

  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      unsigned int input_offset = 4 * (y * width + x);
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

      if (x + 1 < width) addError(x + 1, y,     7.0f/16.0f, r_error, g_error, b_error, a_error);
      if (x > 0 && y + 1 < height) addError(x - 1, y + 1, 3.0f/16.0f, r_error, g_error, b_error, a_error);
      if (y + 1 < height) addError(x,     y + 1, 5.0f/16.0f, r_error, g_error, b_error, a_error);
      if (x + 1 < width && y + 1 < height) addError(x + 1, y + 1, 1.0f/16.0f, r_error, g_error, b_error, a_error);
    }
  }
  
  return unique_ptr<ImageData>(new ImageData(tmp.get(), target_format, width, height));
}
