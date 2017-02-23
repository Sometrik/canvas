#ifndef _DITHERER_H_
#define _DITHERER_H_

#include <InternalFormat.h>

#include <memory>

namespace canvas {
 class ImageData;

 class Ditherer {
  public:
    Ditherer(const ImageData & input_image, InternalFormat _target_format);
    virtual ~Ditherer() = default;

    virtual std::unique_ptr<ImageData> apply() = 0;

  protected:
    inline void addError(int x, int y, float weight, unsigned char r_error, unsigned char g_error, unsigned char b_error, unsigned char a_error) {
      if (x >= 0 && y >= 0 && x < width && y < width) {
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
    }

    std::unique_ptr<unsigned char[]> input_data;
    unsigned int width, height;
    InternalFormat target_format;
  };
};

#endif
