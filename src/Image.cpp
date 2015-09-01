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
      *output_data++ = (alpha << 8) | lum;
    }
  } else {
    for (unsigned int i = 0; i < n; i++) {
      int v = input_data[i];
      int red = RGBA_TO_RED(v) >> 3;
      int green = RGBA_TO_GREEN(v) >> 2;
      int blue = RGBA_TO_BLUE(v) >> 3;

#ifdef __APPLE__
      *output_data++ = PACK_RGB565(blue, green, red);
#else
      *output_data++ = PACK_RGB565(red, green, blue);
#endif
    }
  }

  auto r = std::shared_ptr<Image>(new Image(getWidth(), getHeight(), tmp, target_format));
  delete tmp;
  return r;
}

std::shared_ptr<Image>
Image::scale(unsigned int target_width, unsigned int target_height) {
  assert(target_width < getWidth() && target_height < getHeight());
  unsigned char * target_data = new unsigned char[target_width * target_height * 3];
  for (unsigned int y = 0; y < target_height; y++) {
    for (unsigned int x = 0; x < target_width; x++) {
      int red = 0, green = 0, blue = 0;
      int y0 = y * getHeight() / target_height;
      int y1 = (y + 1) * getHeight() / target_height;
      int x0 = x * getWidth() / target_width;
      int x1 = (x + 1) * getWidth() / target_width;
    }
  }
  auto image2 = new Image(target_width, target_height, target_data, canvas::ImageFormat::RGB24);
  delete[] target_data;
  return std::shared_ptr<Image>(image2);
}
