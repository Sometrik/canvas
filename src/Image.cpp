#include "Image.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace canvas;

std::shared_ptr<Image>
Image::changeFormat(const ImageFormat & target_format) const {
  assert(format.getBytesPerPixel() == 4);
  assert(target_format.getBytesPerPixel() == 2);
  
  unsigned int n = width * height;
  unsigned char * tmp = new unsigned char[target_format.getBytesPerPixel() * n];
  unsigned short * output_data = (unsigned short *)tmp;
  const unsigned int * input_data = (const unsigned int *)data;

  if (target_format.getNumChannels() == 2) {
    for (unsigned int i = 0; i < n; i++) {
      int v = input_data[i];
      int red = RGBA_TO_RED(v);
      int green = RGBA_TO_GREEN(v);
      int blue = RGBA_TO_BLUE(v);
      int alpha = RGBA_TO_ALPHA(v);
      int lum = (red + green + blue) / 3;
      if (lum >= 255) lum = 255;
      *output_data++ = (lum << 8) | alpha;
    }
  } else {
    for (unsigned int i = 0; i < n; i++) {
      int v = input_data[i];
      int red = RGBA_TO_RED(v) >> 3;
      int green = RGBA_TO_GREEN(v) >> 2;
      int blue = RGBA_TO_BLUE(v) >> 3;
      
      *output_data++ = PACK_RGB565(red, green, blue);
    }
  }

  auto r = std::shared_ptr<Image>(new Image(getWidth(), getHeight(), tmp, target_format));
  delete tmp;
  return r;
}
