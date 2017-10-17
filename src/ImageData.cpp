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
