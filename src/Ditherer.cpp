#include <Ditherer.h>

#include <ImageData.h>

using namespace canvas;

Ditherer::Ditherer(const ImageData & input_image, InternalFormat _target_format)
  : target_format(_target_format) {
  width = input_image.getWidth();
  height = input_image.getHeight();
  input_data = std::unique_ptr<unsigned char[]>(new unsigned char[width * height * 4]);
  auto tmp = input_data.get();
  
  auto data = input_image.getData();
  
  auto & fd = input_image.getImageFormat();
  unsigned int n = width * height;
  for (unsigned int i = 0; i < n; i++) {
    unsigned int input_offset = i * fd.getBytesPerPixel();
    unsigned char r = data[input_offset++];
    unsigned char g = fd.getBytesPerPixel() >= 1 ? data[input_offset++] : r;
    unsigned char b = fd.getBytesPerPixel() >= 2 ? data[input_offset++] : g;
    unsigned char a = fd.getBytesPerPixel() >= 3 ? data[input_offset++] : 0xff;
    *tmp++ = r;
    *tmp++ = g;
    *tmp++ = b;
    *tmp++ = a;
  }
}
