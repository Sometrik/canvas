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

  for (unsigned int i = 0; i < n; i++) {
    int v = input_data[i];
    int red = RGBA_TO_RED(v) >> 3;
    int green = RGBA_TO_GREEN(v) >> 2;
    int blue = RGBA_TO_BLUE(v) >> 3;
    
    *output_data++ = PACK_RGB565(red, green, blue);
  }

  return std::shared_ptr<Image>(new Image(getWidth(), getHeight(), tmp, target_format));
}
