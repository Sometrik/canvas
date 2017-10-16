#include "FloydSteinberg.h"

#include <ImageData.h>

#include <cassert>

using namespace std;
using namespace canvas;

FloydSteinberg::FloydSteinberg(const ImageData & input_image, InternalFormat _target_format)
  : target_format(_target_format) {
  width = input_image.getWidth();
  height = input_image.getHeight();
  input_data = std::unique_ptr<unsigned char[]>(new unsigned char[width * height * 4]);
  auto tmp = input_data.get();
  
  auto data = input_image.getData();

  auto & fd = input_image.getImageFormat();
  unsigned int n = width * height;

  if (fd.getBytesPerPixel() == 4) {
    memcpy(tmp, data, 4 * n);
  } else if (fd.getBytesPerPixel() == 3) {
    for (unsigned int offset = 0; offset < 3 * n; ) {
      *tmp++ = data[offset++];
      *tmp++ = data[offset++];
      *tmp++ = data[offset++];
      *tmp++ = 0xff;
    }
  } else if (fd.getBytesPerPixel() == 1) {
    for (unsigned int offset = 0; offset < n; ) {
      unsigned char v = data[offset++];
      *tmp++ = v;
      *tmp++ = v;
      *tmp++ = v;
      *tmp++ = 0xff;
    }
  }
}

std::unique_ptr<ImageData>
FloydSteinberg::apply() {
  auto target_size = width * height * 2;
  std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_size]);
  unsigned short * output_data = (unsigned short *)tmp.get();
  unsigned int n = target_size / 2;
  unsigned int * input = (unsigned int *)input_data.get();

  if (target_format == RGBA4) {
    for (unsigned int y = 0; y < (int)height; y++) {
      for (unsigned int x = 0; x < (int)width; x++, input++) {
        unsigned int v = *input;
        unsigned int error = v & 0x0f0f0f0f;
#if defined __APPLE__ || defined __ANDROID__
        *output_data++ = ((RGBA_TO_RED(v) >> 4) << 12) | ((RGBA_TO_GREEN(v) >> 4) << 8) | ((RGBA_TO_BLUE(v) >> 4) << 4) | (RGBA_TO_ALPHA(v) >> 4);
#else
        *output_data++ = ((RGBA_TO_BLUE(v) >> 4) << 12) | ((RGBA_TO_GREEN(v) >> 4) << 8) | ((RGBA_TO_RED(v) >> 4) << 4) | (RGBA_TO_ALPHA(v) >> 4);
#endif
        if (x + 1 < width) addErrorAlpha(x + 1, y,     7, 16, error);
        if (x > 0 && y + 1 < height) addErrorAlpha(x - 1, y + 1, 3, 16, error);
        if (y + 1 < height) addErrorAlpha(x,     y + 1, 5, 16, error);
        if (x + 1 < width && y + 1 < height) addErrorAlpha(x + 1, y + 1, 1, 16, error);
      }
    }
  } else {
    for (unsigned int y = 0; y < (int)height; y++) {
      for (unsigned int x = 0; x < (int)width; x++, input++) {
        unsigned int v = *input;
        unsigned int error = v & 0x07030700;
#if defined __APPLE__ || defined __ANDROID__
        *output_data++ = PACK_RGB565(RGBA_TO_BLUE(v) >> 3, RGBA_TO_GREEN(v) >> 2, RGBA_TO_RED(v) >> 3);
#else
        *output_data++ = PACK_RGB565(RGBA_TO_RED(v) >> 3, RGBA_TO_GREEN(v) >> 2, RGBA_TO_BLUE(v) >> 3);
#endif
        if (x + 1 < width) addError(x + 1, y,     7, 16, error);
        if (x > 0 && y + 1 < height) addError(x - 1, y + 1, 3, 16, error);
        if (y + 1 < height) addError(x,     y + 1, 5, 16, error);
        if (x + 1 < width && y + 1 < height) addError(x + 1, y + 1, 1, 16, error);
      }
    }
  }

  return std::unique_ptr<ImageData>(new ImageData(tmp.get(), target_format, width, height));
}
