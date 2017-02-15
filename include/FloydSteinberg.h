#ifndef _FLOYDSTEINBERG_H_
#define _FLOYDSTEINBERG_H_

#include <ImageData.h>

namespace canvas {
  class FloydSteinberg {
  public:
    FloydSteinberg(const ImageData * _image, InternalFormat target_format);

    std::unique_ptr<ImageData> apply();
    
  private:
    inline void addError(unsigned int x, unsigned int y, float weight, unsigned char r_error, unsigned char g_error, unsigned char b_error, unsigned char a_error) {
      unsigned int offset = 4 * (y * width + x);
      int r = int(input_data[offset++] + weight * r_error);
      int g = int(input_data[offset++] + weight * g_error);
      int b = int(input_data[offset++] + weight * b_error);
      int a = int(input_data[offset++] + weight * a_error);
      if (r > 0xff) r = 0xff;
      if (g > 0xff) g = 0xff;
      if (b > 0xff) b = 0xff;
      if (a > 0xff) a = 0xff;
      offset -= 4;
      input_data[offset++] = r;
      input_data[offset++] = g;
      input_data[offset++] = b;
      input_data[offset++] = a;
    }
    
    std::unique_ptr<unsigned char[]> input_data;
    unsigned int width, height;
    InternalFormat target_format;
  };
};

#endif
