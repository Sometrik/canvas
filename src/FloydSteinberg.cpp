#include "FloydSteinberg.h"

#include <ImageData.h>
#include <ImageFormat.h>

#include <cassert>

using namespace std;
using namespace canvas;

static inline void addErrorAlpha(unsigned int * input_data, unsigned int width, unsigned int x, unsigned int y, int weight, unsigned int error) {
  unsigned int offset = y * width + x;
  unsigned int value = input_data[offset];
  int r = RGBA_TO_RED(value) + weight * RGBA_TO_RED(error) / 16;
  int g = RGBA_TO_GREEN(value) + weight * RGBA_TO_GREEN(error) / 16;
  int b = RGBA_TO_BLUE(value) + weight * RGBA_TO_BLUE(error) / 16;
  int a = RGBA_TO_ALPHA(value) + weight * RGBA_TO_ALPHA(error) / 16;
  if (r > 0xff) r = 0xff;
  if (g > 0xff) g = 0xff;
  if (b > 0xff) b = 0xff;
  if (a > 0xff) a = 0xff;
  input_data[offset] = PACK_RGBA32(r, g, b, a);
}

static inline void addError(unsigned int * input_data, unsigned int width, unsigned int x, unsigned int y, int weight, unsigned int error) {
  unsigned int offset = y * width + x;
  unsigned int value = input_data[offset];
  int r = RGBA_TO_RED(value) + weight * RGBA_TO_RED(error) / 16;
  int g = RGBA_TO_GREEN(value) + weight * RGBA_TO_GREEN(error) / 16;
  int b = RGBA_TO_BLUE(value) + weight * RGBA_TO_BLUE(error) / 16;
  if (r > 0xff) r = 0xff;
  if (g > 0xff) g = 0xff;
  if (b > 0xff) b = 0xff;
  input_data[offset] = PACK_RGBA32(r, g, b, 0);
}

static std::unique_ptr<unsigned char[]> apply2(unsigned int * input_data, unsigned int width, unsigned int height, InternalFormat target_format) {
  unsigned int * input = input_data;
  auto target_size = width * height * 2;
  std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_size]);
  unsigned short * output_data = (unsigned short *)tmp.get();

  if (target_format == RGBA4) {
    for (unsigned int y = 0; y < height; y++) {
      for (unsigned int x = 0; x < width; x++, input++) {
        unsigned int v = *input;
        unsigned int error = v & 0x0f0f0f0f;
#if defined __APPLE__ || defined __ANDROID__
        *output_data++ = ((RGBA_TO_RED(v) >> 4) << 12) | ((RGBA_TO_GREEN(v) >> 4) << 8) | ((RGBA_TO_BLUE(v) >> 4) << 4) | (RGBA_TO_ALPHA(v) >> 4);
#else
        *output_data++ = ((RGBA_TO_BLUE(v) >> 4) << 12) | ((RGBA_TO_GREEN(v) >> 4) << 8) | ((RGBA_TO_RED(v) >> 4) << 4) | (RGBA_TO_ALPHA(v) >> 4);
#endif
        if (x + 1 < width) addErrorAlpha(input_data, width, x + 1, y, 7, error);
        if (x > 0 && y + 1 < height) addErrorAlpha(input_data, width, x - 1, y + 1, 3, error);
        if (y + 1 < height) addErrorAlpha(input_data, width, x, y + 1, 5, error);
        if (x + 1 < width && y + 1 < height) addErrorAlpha(input_data, width, x + 1, y + 1, 1, error);
      }
    }
  } else {
    for (unsigned int y = 0; y < height; y++) {
      for (unsigned int x = 0; x < width; x++, input++) {
        unsigned int v = *input;
        unsigned int error = v & 0x07030700;
#if defined __APPLE__ || defined __ANDROID__
        *output_data++ = PACK_RGB565(RGBA_TO_BLUE(v) >> 3, RGBA_TO_GREEN(v) >> 2, RGBA_TO_RED(v) >> 3);
#else
        *output_data++ = PACK_RGB565(RGBA_TO_RED(v) >> 3, RGBA_TO_GREEN(v) >> 2, RGBA_TO_BLUE(v) >> 3);
#endif
        if (x + 1 < width) addError(input_data, width, x + 1, y, 7, error);
        if (x > 0 && y + 1 < height) addError(input_data, width, x - 1, y + 1, 3, error);
        if (y + 1 < height) addError(input_data, width, x, y + 1, 5, error);
        if (x + 1 < width && y + 1 < height) addError(input_data, width, x + 1, y + 1, 1, error);
      }
    }
  }

  return tmp;
}

std::unique_ptr<unsigned char[]>
FloydSteinberg::apply(const ImageData & input_image) const {
  unsigned int width = input_image.getWidth();
  unsigned int height = input_image.getHeight();

  auto data = input_image.getData();

  auto input_data = std::unique_ptr<unsigned int[]>(new unsigned int[width * height]);

  if (input_image.getNumChannels() == 4) {
    memcpy(input_data.get(), data, 4 * width * height);
  } if (input_image.getNumChannels() == 3) {
    auto tmp = (unsigned char *)input_data.get();
    for (unsigned int offset = 0; offset < 3 * width * height; ) {
      *tmp++ = data[offset++];
      *tmp++ = data[offset++];
      *tmp++ = data[offset++];
      *tmp++ = 0xff;
    }
  } else if (input_image.getNumChannels() == 1) {
    auto tmp = (unsigned char *)input_data.get();
    for (unsigned int offset = 0; offset < width * height; ) {
      unsigned char v = data[offset++];
      *tmp++ = v;
      *tmp++ = v;
      *tmp++ = v;
      *tmp++ = 0xff;
    }
  }

  return apply2(input_data.get(), width, height, target_format);
}
