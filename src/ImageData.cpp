#include <ImageData.h>

#include <cassert>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

using namespace std;
using namespace canvas;

ImageData ImageData::nullImage;

std::unique_ptr<ImageData>
ImageData::scale(unsigned int target_width, unsigned int target_height) const {
  size_t target_size = calculateSize(target_width, target_height, num_channels);

  std::unique_ptr<unsigned char[]> output_data(new unsigned char[target_size]);

  stbir_resize_uint8(data.get(), getWidth(), getHeight(), 0, output_data.get(), target_width, target_height, 0, num_channels);

  return unique_ptr<ImageData>(new ImageData(output_data.get(), target_width, target_height, num_channels));
}

std::unique_ptr<ImageData>
ImageData::colorize(const Color & color) const {
  assert(num_channels == 1);

  int red = int(255 * color.red * color.alpha);
  int green = int(255 * color.green * color.alpha);
  int blue = int(255 * color.blue * color.alpha);
  int alpha = int(255 * color.alpha);

  unique_ptr<ImageData> r(new ImageData(width, height, 4));

  unsigned char * target_buffer = r->getData();
  for (unsigned int i = 0; i < width * height; i++) {
    unsigned char v = data[i];
    target_buffer[4 * i + 0] = (unsigned char)(red * v / 255);
    target_buffer[4 * i + 1] = (unsigned char)(green * v / 255);
    target_buffer[4 * i + 2] = (unsigned char)(blue * v / 255);
    target_buffer[4 * i + 3] = (unsigned char)(alpha * v / 255);
  }

  return r;
}
