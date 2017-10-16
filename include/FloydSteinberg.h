#ifndef _FLOYDSTEINBERG_H_
#define _FLOYDSTEINBERG_H_

#include <InternalFormat.h>
#include <ImageFormat.h>
#include <memory>

namespace canvas {
  class ImageData;

  class FloydSteinberg  {
  public:
    FloydSteinberg(const ImageData & input_image, InternalFormat _target_format);
    std::unique_ptr<ImageData> apply();

  private:
    inline void addErrorAlpha(unsigned int x, unsigned int y, int weight, int divisor, unsigned int error) {
      unsigned int offset = 4 * (y * width + x);
      int r = input_data[offset + 0] + weight * RGBA_TO_RED(error) / divisor;
      int g = input_data[offset + 1] + weight * RGBA_TO_GREEN(error) / divisor;
      int b = input_data[offset + 2] + weight * RGBA_TO_BLUE(error) / divisor;
      int a = input_data[offset + 3] + weight * RGBA_TO_ALPHA(error) / divisor;
      if (r > 0xff) r = 0xff;
      if (g > 0xff) g = 0xff;
      if (b > 0xff) b = 0xff;
      if (a > 0xff) a = 0xff;
      input_data[offset + 0] = r;
      input_data[offset + 1] = g;
      input_data[offset + 2] = b;
      input_data[offset + 3] = a;
    }

    inline void addError(unsigned int x, unsigned int y, int weight, int divisor, unsigned int error) {
      unsigned int offset = 4 * (y * width + x);
      int r = input_data[offset + 0] + weight * RGBA_TO_RED(error) / divisor;
      int g = input_data[offset + 1] + weight * RGBA_TO_GREEN(error) / divisor;
      int b = input_data[offset + 2] + weight * RGBA_TO_BLUE(error) / divisor;
      if (r > 0xff) r = 0xff;
      if (g > 0xff) g = 0xff;
      if (b > 0xff) b = 0xff;
      input_data[offset + 0] = r;
      input_data[offset + 1] = g;
      input_data[offset + 2] = b;
    }

    std::unique_ptr<unsigned char[]> input_data;
    unsigned int width, height;
    InternalFormat target_format;
  };
};

#endif
