#include "ImageFormat.h"

using namespace canvas;

ImageFormat ImageFormat::UNDEF(0, 0);
ImageFormat ImageFormat::RGB24(3, 3);
ImageFormat ImageFormat::RGB32(3, 4);
ImageFormat ImageFormat::RGBA32(4, 4);
ImageFormat ImageFormat::RGB565(3, 2);
ImageFormat ImageFormat::PAL8(3, 1);
ImageFormat ImageFormat::LUM8(1, 1);
ImageFormat ImageFormat::ALPHA8(1, 1, true);
ImageFormat ImageFormat::LUMALPHA8(2, 2, true);

ImageFormat::ImageFormat(unsigned short _channels, unsigned short _bytes_per_pixel, bool _force_alpha)
  : channels(_channels),
    bytes_per_pixel(_bytes_per_pixel),
    force_alpha(_force_alpha)
{
  
}

bool
ImageFormat::operator==(const ImageFormat & other) {
  return channels == other.channels && bytes_per_pixel == other.bytes_per_pixel && force_alpha == other.force_alpha;
}
