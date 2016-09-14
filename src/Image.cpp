#include <Image.h>

#include <cassert>
#include <iostream>

#include "rg_etc1.h"
#include "dxt.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

using namespace std;
using namespace canvas;

bool Image::etc1_initialized = false;

Image::Image(InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels, short _quality) : width(_width), height(_height), levels(_levels), format(_format), quality(_quality) {
  size_t s = calculateSize();

  auto & fd = getImageFormat(format);
  
  data = new unsigned char[s];
  if (fd.getCompression() == ImageFormat::ETC1) {
    for (unsigned int i = 0; i < s; i += 8) {
      *(unsigned int *)(data + i + 0) = 0x00000000;
      *(unsigned int *)(data + i + 4) = 0xffffffff;
    }
  } else if (fd.getCompression() == ImageFormat::DXT1) {
    for (unsigned int i = 0; i < s; i += 8) {
      *(unsigned int *)(data + i + 0) = 0x00000000;
      *(unsigned int *)(data + i + 4) = 0xaaaaaaaa;
    }
  } else if (fd.getCompression() == ImageFormat::RGTC1) {
    for (unsigned int i = 0; i < s; i += 8) {
      *(unsigned int *)(data + i + 0) = 0x00000003; // doesn't work on big endian
      *(unsigned int *)(data + i + 4) = 0x00000000;
    }
  } else if (fd.getCompression() == ImageFormat::RGTC2) {
    for (unsigned int i = 0; i < s; i += 16) {
      *(unsigned int *)(data + i + 0) = 0x00000003;
      *(unsigned int *)(data + i + 4) = 0x00000000;
      *(unsigned int *)(data + i + 4) = 0x00000003;
      *(unsigned int *)(data + i + 8) = 0x00000000;
    }
  } else if (!fd.getCompression()) {
    cerr << "clearing memory for " << s << " bytes\n";
    memset(data, 0, s);      
  } else {
    assert(0);
  }
}

std::shared_ptr<Image>
Image::convert(InternalFormat target_format) const {
  auto & fd = getImageFormat(format);
  auto & target_fd = getImageFormat(target_format);
  
  assert(fd.getBytesPerPixel() == 4);
  assert(!fd.getCompression());

  if (target_fd.getCompression() == ImageFormat::DXT1 || target_fd.getCompression() == ImageFormat::ETC1 || target_fd.getCompression() == ImageFormat::RGTC1 || target_fd.getCompression() == ImageFormat::RGTC2) {
    rg_etc1::etc1_pack_params params;
    params.m_quality = rg_etc1::cLowQuality;
    if (target_fd.getCompression() == ImageFormat::ETC1 && !etc1_initialized) {
      cerr << "initializing etc1" << endl;
      etc1_initialized = true;
      rg_etc1::pack_etc1_block_init();
    }
    assert((width & 3) == 0);
    assert((height & 3) == 0);
    unsigned int target_width = width, target_height = height;
    unsigned int target_size = calculateSize(target_width, target_height, levels, target_format);
    std::unique_ptr<unsigned char[]> output_data(new unsigned char[target_size]);
    unsigned char input_block[4*4*8];
    unsigned int target_offset = 0;
    for (unsigned int level = 0; level < levels; level++) {
      unsigned int rows = (target_height + 3) / 4, cols = (target_width + 3) / 4;
      int base_source_offset = calculateOffset(level);
      // cerr << "compressing texture, level " << level << ", rows = " << rows << ", cols = " << cols << "\n";
      for (unsigned int row = 0; row < rows; row++) {
	for (unsigned int col = 0; col < cols; col++) {
	  for (unsigned int y = 0; y < 4; y++) {
	    for (unsigned int x = 0; x < 4; x++) {
	      int source_offset = base_source_offset + ((row * 4 + y) * target_width + col * 4 + x) * 4;
	      if (target_fd.getCompression() == ImageFormat::ETC1) {
		int offset = (y * 4 + x) * 4;
		input_block[offset++] = data[source_offset++];
		input_block[offset++] = data[source_offset++];
		input_block[offset++] = data[source_offset++];
		input_block[offset++] = 255; // data[source_offset++];
	      } else if (target_fd.getCompression() == ImageFormat::DXT1) {
		int offset = (y * 4 + x) * 4;
		input_block[offset++] = data[source_offset + 2];
		input_block[offset++] = data[source_offset + 1];
		input_block[offset++] = data[source_offset + 0];
		input_block[offset++] = 255; // data[source_offset++];
	      } else if (target_fd.getCompression() == ImageFormat::RGTC1) {
		int offset = y * 4 + x;
		input_block[offset] = data[source_offset + 0];		
	      } else {
		int offset = y * 4 + x;
		input_block[offset] = data[source_offset + 0];
		input_block[offset + 16] = data[source_offset + 3];
	      }
	    }
	  }
	  if (target_fd.getCompression() == ImageFormat::ETC1) {
	    rg_etc1::pack_etc1_block(output_data.get() + target_offset, (const unsigned int *)&(input_block[0]), params);	  
	    target_offset += 8;
	  } else if (target_fd.getCompression() == ImageFormat::DXT1) {
	    stb_compress_dxt1_block(output_data.get() + target_offset, &(input_block[0]), false, 2);
	    target_offset += 8;
	  } else if (target_fd.getCompression() == ImageFormat::RGTC1) {
	    stb_compress_rgtc1_block(output_data.get() + target_offset, &(input_block[0]));
	    target_offset += 8;
	  } else {
	    stb_compress_rgtc2_block(output_data.get() + target_offset, &(input_block[0]));
	    target_offset += 16;
	  }
	}
      }
      target_width = (target_width + 1) / 2;
      target_height = (target_height + 1) / 2;
    }
    return make_shared<Image>(output_data.get(), target_format, width, height, levels);
  } else if (target_fd.getNumChannels() == 2 && target_fd.getBytesPerPixel() == 1) {
    assert(levels == 1);
    
    unsigned int n = width * height;
    std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_fd.getBytesPerPixel() * n]);
    unsigned char * output_data = (unsigned char *)tmp.get();
    const unsigned int * input_data = (const unsigned int *)data;
    
    for (unsigned int i = 0; i < n; i++) {
      int v = input_data[i];
      int red = RGBA_TO_RED(v);
      int green = RGBA_TO_GREEN(v);
      int blue = RGBA_TO_BLUE(v);
      int alpha = RGBA_TO_ALPHA(v) >> 4;
      int lum = ((red + green + blue) / 3) >> 4;
      if (lum >= 16) lum = 15;
      *output_data++ = (alpha << 4) | lum;
    }

    return make_shared<Image>(tmp.get(), target_format, getWidth(), getHeight());
  } else {
    assert(target_fd.getBytesPerPixel() == 2);
    unsigned int target_size = calculateSize(getWidth(), getHeight(), getLevels(), target_format);
    std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_size]);
    unsigned short * output_data = (unsigned short *)tmp.get();
    const unsigned int * input_data = (const unsigned int *)data;
    unsigned int n = calculateSize() / fd.getBytesPerPixel();
    if (target_fd.getNumChannels() == 2) {
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
    } else if (target_fd.getNumChannels() == 3) {
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
    } else {
      for (unsigned int i = 0; i < n; i++) {
	int v = input_data[i];
	int red = RGBA_TO_RED(v) >> 4;
	int green = RGBA_TO_GREEN(v) >> 4;
	int blue = RGBA_TO_BLUE(v) >> 4;
	int alpha = RGBA_TO_ALPHA(v) >> 4;
	*output_data++ = (red << 12) | (green << 8) | (blue << 4) | alpha;
      }
    }
    
    return make_shared<Image>(tmp.get(), target_format, getWidth(), getHeight(), getLevels());
  }
}

std::shared_ptr<Image>
Image::scale(unsigned int target_base_width, unsigned int target_base_height, unsigned int target_levels) const {
  auto & fd = getImageFormat(format);
  assert(fd.getBytesPerPixel() == 4);
  assert(!fd.getCompression());
  size_t input_size = calculateSize();
  size_t target_size = calculateOffset(target_base_width, target_base_height, target_levels, format);
  // cerr << "scaling to " << target_base_width << " " << target_base_height << " " << target_levels << " => " << target_size << " bytes\n";
  std::unique_ptr<unsigned char[]> output_data(new unsigned char[target_size]);
  unsigned int target_offset = 0, target_width = target_base_width, target_height = target_base_height;
#if 1
  stbir_resize_uint8(data, getWidth(), getHeight(), 0, output_data.get(), target_width, target_height, 0, fd.getBytesPerPixel());

#else
  for (int y = 0; y < int(target_height); y++) {
    for (int x = 0; x < int(target_width); x++) {
      int red = 0, green = 0, blue = 0, alpha = 0, n = 0;
      int y0 = y * getHeight() / target_height;
      int y1 = (y + 1) * getHeight() / target_height;
      int x0 = x * getWidth() / target_width;
      int x1 = (x + 1) * getWidth() / target_width;
      if (y0 == y1 && y1 < height) y1++;
      if (x0 == x1 && x1 < width) x1++;
      for (int j = y0; j < y1; j++) {
	for (int k = x0; k < x1; k++) {
          int offset = (j * getWidth() + k) * 4;
          assert(offset + 4 <= input_size);
          red += data[offset++];
	  green += data[offset++];
	  blue += data[offset++];
	  alpha += data[offset++];
	  n++;
	}
      }
      if (n) {
        red /= n;
        green /= n;
        blue /= n;
        alpha /= n;
      }
      output_data[target_offset++] = (unsigned char)(red);
      output_data[target_offset++] = (unsigned char)(green);
      output_data[target_offset++] = (unsigned char)(blue);
      output_data[target_offset++] = (unsigned char)(alpha);
    }
  }
#endif

  if (target_levels > 1) {
    unsigned int source_width = target_width, source_height = target_height;
    target_width /= 2;
    target_height /= 2;
    
    for (unsigned int level = 1; level < target_levels; level++) {
      unsigned char * source_data = output_data.get() + calculateOffset(target_base_width, target_base_height, level - 1, format);
      unsigned char * target_data = output_data.get() + calculateOffset(target_base_width, target_base_height, level, format);
      for (int y = 0; y < target_height; y++) {               
	for (int x = 0; x < target_width; x++) {
	  unsigned int source_offset = (2 * y * source_width + 2 * x) * 4;
	  unsigned int target_offset = (y * target_width + x) * 4;
	  for (unsigned int c = 0; c < 4; c++) {
	    target_data[target_offset] = ((int)source_data[source_offset]  + 
					  (int)source_data[source_offset + 4]  + 
					  (int)source_data[source_offset + 4 + 4 * source_width] +
					  (int)source_data[source_offset + 4 * source_width]) / 4; 
	    source_offset++;
	    target_offset++;
	  }
	}
      }
      source_width = target_width;
      source_height = target_height;
      target_width /= 2;
      target_height /= 2;
    }
  }
  return make_shared<Image>(output_data.get(), getInternalFormat(), target_base_width, target_base_height, target_levels);
}

std::shared_ptr<Image>
Image::createMipmaps(unsigned int target_levels) const {
  auto & fd = getImageFormat(format);
  assert(fd.getBytesPerPixel() == 4);
  assert(!fd.getCompression());
  assert(levels == 1);
  size_t target_size = calculateOffset(target_levels);
  std::unique_ptr<unsigned char[]> output_data(new unsigned char[target_size]);
  memcpy(output_data.get(), data, calculateOffset(1));
  unsigned int source_width = width, source_height = height;
  unsigned int target_width = width / 2, target_height = height / 2;
  for (unsigned int level = 1; level < target_levels; level++) {
    unsigned char * source_data = output_data.get() + calculateOffset(level - 1);
    unsigned char * target_data = output_data.get() + calculateOffset(level);
    for (int y = 0; y < target_height; y++) {               
      for (int x = 0; x < target_width; x++) {
	unsigned int source_offset = (2 * y * source_width + 2 * x) * 4;
	unsigned int target_offset = (y * target_width + x) * 4;
	for (unsigned int c = 0; c < 4; c++) {
	  target_data[target_offset] = ((int)source_data[source_offset]  + 
					(int)source_data[source_offset + 4]  + 
					(int)source_data[source_offset + 4 + 4 * source_width] +
					(int)source_data[source_offset + 4 * source_width]) / 4; 
	  source_offset++;
	  target_offset++;
	}
      }
    }
    source_width = target_width;
    source_height = target_height;
    target_width /= 2;
    target_height /= 2;
  }
  return make_shared<Image>(output_data.get(), getInternalFormat(), width, height, target_levels);
}
