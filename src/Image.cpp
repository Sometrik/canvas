#include "Image.h"

#include <cassert>
#include <iostream>

#include "rg_etc1.h"

using namespace std;
using namespace canvas;

bool Image::etc1_initialized = false;

std::shared_ptr<Image>
Image::changeFormat(const ImageFormat & target_format) const {
  assert(format.getBytesPerPixel() == 4);

  if (target_format.getCompression() == ImageFormat::ETC1) {
    rg_etc1::etc1_pack_params params;
    if (!etc1_initialized) {
      etc1_initialized = true;
      rg_etc1::pack_etc1_block_init();
    }
    assert((width & 3) == 0);
    assert((height & 3) == 0);
    unsigned int rows = height >> 2, cols = width >> 2;
    unsigned char * output_data = new unsigned char[rows * cols * 8];
    const unsigned int * input_data = (const unsigned int *)data;
    unsigned int input_block[4*4];
    for (unsigned int row = 0; row < rows; row++) {
      for (unsigned int col = 0; col < cols; col++) {
	for (unsigned int y = 0; y < 4; y++) {
	  memcpy(input_block + y * 4, input_data + (row * 4 + y) * width + col, 4*4);
	}
	rg_etc1::pack_etc1_block(output_data + (row * rows + col) * 8, &(input_block[0]), params);
      }
    }
    auto r = std::shared_ptr<Image>(new Image(getWidth(), getHeight(), output_data, target_format));
    delete[] output_data;
    return r;
  } else {
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
    delete[] tmp;
    return r;
  }
}

std::shared_ptr<Image>
Image::scale(unsigned int target_width, unsigned int target_height) const {
  unsigned char * target_data = new unsigned char[target_width * target_height * 4];
  unsigned short bpp = format.getBytesPerPixel();
  int target_offset = 0;
  for (int y = 0; y < int(target_height); y++) {
    for (int x = 0; x < int(target_width); x++) {
      int red = 0, green = 0, blue = 0, n = 0;
      int y0 = y * getHeight() / target_height;
      int y1 = (y + 1) * getHeight() / target_height;
      int x0 = x * getWidth() / target_width;
      int x1 = (x + 1) * getWidth() / target_width;
      if (y0 == y1) y1++;
      if (x0 == x1) x1++;
      for (int j = y0; j < y1; j++) {
	for (int k = x0; k < x1; k++) {
	  int offset = (j * getHeight() + k) * bpp;
	  red += data[offset++];
	  green += data[offset++];
	  blue += data[offset++];
	  n++;
	}
      }
      target_data[target_offset++] = (unsigned char)(red / n);
      target_data[target_offset++] = (unsigned char)(green / n);
      target_data[target_offset++] = (unsigned char)(blue / n);
      target_data[target_offset++] = 0xff;
    }
  }
  std::shared_ptr<Image> image2(new Image(target_width, target_height, target_data, canvas::ImageFormat::RGB32));
  delete[] target_data;
  return image2;
}
