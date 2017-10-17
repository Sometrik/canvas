#include <PackedImageData.h>

#include <FloydSteinberg.h>
#include <ImageData.h>

#include "rg_etc1.h"
#include "dxt.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace canvas;

bool PackedImageData::etc1_initialized = false;

PackedImageData::PackedImageData(InternalFormat _format, unsigned int _levels, const ImageData & input)
  : format(_format), width(input.getWidth()), height(input.getHeight()), levels(_levels)
{
  if (format == NO_FORMAT) {
    if (input.getNumChannels() == 4) format = RGBA8;
    else if (input.getNumChannels() == 3) format = RGB8;
    else if (input.getNumChannels() == 2) format = RG8;
    else if (input.getNumChannels() == 1) format = R8;
    else {
      assert(0);
    }
  }

  size_t s = calculateSize();
  data = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
    
  if (format == RGBA4 || format == RGB565) {
    FloydSteinberg fs(format);
    unsigned int offset = fs.apply(input, data.get());
    if (levels >= 2) {
      auto img = input.scale((input.getWidth() + 1) / 2, (input.getHeight() + 1) / 2);
      for (int l = 1; l < levels; l++) {
	offset += fs.apply(*img, data.get() + offset);
	if (l + 1 < levels) {
	  img = img->scale((img->getWidth() + 1) / 2, (img->getHeight() + 1) / 2);
	}
      }
    }
  } else {
    const unsigned char * input_data = input.getData();
    unsigned char * output_data = (unsigned char *)data.get();
    unsigned int num_channels = input.getNumChannels();
    
    if (format == RGB8 || format == RGBA8) {
      assert(levels == 1);
      if (num_channels == 4) {
	memcpy(output_data, input_data, s);
      } else {
	unsigned int n = width * height;
	for (unsigned int i = 0; i < n; i++) {
	  unsigned char r = *input_data++;
	  unsigned char g = num_channels >= 2 ? *input_data++ : r;
	  unsigned char b = num_channels >= 3 ? *input_data++ : g;
	  unsigned char a = num_channels >= 4 ? *input_data++ : 0xff;
#if 1
	  // defined __APPLE__ || defined __ANDROID__
	  *output_data++ = b;
	  *output_data++ = g;
	  *output_data++ = r;
#else
	  *output_data++ = r;
	  *output_data++ = g;
	  *output_data++ = b;
#endif
	  *output_data++ = a;
	}	
      }
    } else if (format == LA44) {
      assert(levels == 1);
      unsigned int n = width * height;
      
      for (unsigned int i = 0; i < n; i++) {
	unsigned int input_offset = i * num_channels;
	unsigned char r = input_data[input_offset++];
	unsigned char g = num_channels >= 2 ? input_data[input_offset++] : r;
	unsigned char b = num_channels >= 3 ? input_data[input_offset++] : g;
	unsigned char a = (num_channels >= 4 ? input_data[input_offset++] : 0xff) >> 4;
	int lum = ((r + g + b) / 3) >> 4;
	if (lum >= 16) lum = 15;
	*output_data++ = (a << 4) | lum;
      }
    } else {
      cerr << "unable to pack input data (channels = " << input.getNumChannels() << ", f = " << int(format) << ")\n";
      assert(0);
    }
  }
}

PackedImageData::PackedImageData(InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels)
  : width(_width), height(_height), levels(_levels), format(_format) {
  size_t s = calculateSize();
  data = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
  if (format == RGB_ETC1) {
    for (unsigned int i = 0; i < s; i += 8) {
      *(unsigned int *)(data.get() + i + 0) = 0x00000000;
      *(unsigned int *)(data.get() + i + 4) = 0xffffffff;
    }
  } else if (format == RGB_DXT1) {
    for (unsigned int i = 0; i < s; i += 8) {
      *(unsigned int *)(data.get() + i + 0) = 0x00000000;
      *(unsigned int *)(data.get() + i + 4) = 0xaaaaaaaa;
    }
  } else if (format == RED_RGTC1) {
    for (unsigned int i = 0; i < s; i += 8) {
      *(unsigned int *)(data.get() + i + 0) = 0x00000003; // doesn't work on big endian
      *(unsigned int *)(data.get() + i + 4) = 0x00000000;
    }
  } else if (format == RG_RGTC2) {
    for (unsigned int i = 0; i < s; i += 16) {
      *(unsigned int *)(data.get() + i + 0) = 0x00000003;
      *(unsigned int *)(data.get() + i + 4) = 0x00000000;
      *(unsigned int *)(data.get() + i + 8) = 0x00000003;
      *(unsigned int *)(data.get() + i + 12) = 0x00000000;
    }
  } else {
    memset(data.get(), 0, s);
  }
}

#if 0
void
PackedImageData::createMipmaps(const ImageData & input_data, unsigned int target_levels) const {
  auto & fd = getImageFormat(format);
  assert(fd.getBytesPerPixel() == 4);
  assert(!fd.getCompression());
  assert(levels == 1);
  size_t target_size = calculateOffset(target_levels);
  std::unique_ptr<unsigned char[]> output_data(new unsigned char[target_size]);
  memcpy(output_data.get(), data.get(), calculateOffset(1));
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
  return unique_ptr<ImageData>(new ImageData(output_data.get(), getInternalFormat(), width, height, target_levels));
}

std::unique_ptr<ImageData>
ImageData::convert(InternalFormat target_format) const {
  auto & fd = getImageFormat(format);
  
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
	      int source_offset = base_source_offset + ((row * 4 + y) * target_width + col * 4 + x) * fd.getBytesPerPixel();
	      unsigned char r = data[source_offset++];
	      unsigned char g = fd.getBytesPerPixel() >= 2 ? data[source_offset++] : r;
	      unsigned char b = fd.getBytesPerPixel() >= 3 ? data[source_offset++] : g;
	      unsigned char a = fd.getBytesPerPixel() >= 4 ? data[source_offset++] : 0xff;
	      if (target_fd.getCompression() == ImageFormat::ETC1) {
		int offset = (y * 4 + x) * 4;
		input_block[offset++] = r;
		input_block[offset++] = g;
		input_block[offset++] = b;
		input_block[offset++] = 255; // data[source_offset++];
	      } else if (target_fd.getCompression() == ImageFormat::DXT1) {
		int offset = (y * 4 + x) * 4;
		input_block[offset++] = b;
		input_block[offset++] = g;
		input_block[offset++] = r;
		input_block[offset++] = 255; // data[source_offset++];
	      } else if (target_fd.getCompression() == ImageFormat::RGTC1) {
		int offset = y * 4 + x;
		input_block[offset] = r;
	      } else {
		int offset = y * 4 + x;
		input_block[offset] = r;
		input_block[offset + 16] = a;
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
    return unique_ptr<ImageData>(new ImageData(output_data.get(), target_format, width, height, levels));
  } else if (target_fd.getNumChannels() == 1) {
    assert(levels == 1);
    
    unsigned int n = width * height;
    std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_fd.getBytesPerPixel() * n]);
    unsigned char * output_data = (unsigned char *)tmp.get();
    
    for (unsigned int i = 0; i < n; i++) {
      unsigned int input_offset = i * fd.getBytesPerPixel();
      unsigned char r = data[input_offset++];
      unsigned char g = fd.getBytesPerPixel() >= 1 ? data[input_offset++] : r;
      unsigned char b = fd.getBytesPerPixel() >= 2 ? data[input_offset++] : g;
      *output_data++ = (r + g + b) / 3;
    }

    return unique_ptr<ImageData>(new ImageData(tmp.get(), target_format, getWidth(), getHeight()));
  } else {
    assert(target_fd.getBytesPerPixel() == 2);
    auto target_size = calculateSize(getWidth(), getHeight(), getLevels(), target_format);
    std::unique_ptr<unsigned char[]> tmp(new unsigned char[target_size]);
    unsigned short * output_data = (unsigned short *)tmp.get();
    auto n = calculateSize() / fd.getBytesPerPixel();
    if (target_fd.getNumChannels() == 2) {
      for (auto i = 0; i < n; i++) {
	unsigned int input_offset = i * fd.getBytesPerPixel();
	unsigned char r = data[input_offset++];
	unsigned char g = fd.getBytesPerPixel() >= 1 ? data[input_offset++] : r;
	unsigned char b = fd.getBytesPerPixel() >= 2 ? data[input_offset++] : g;
	unsigned char a = fd.getBytesPerPixel() >= 3 ? data[input_offset++] : 0xff;
	int lum = (r + g + b) / 3;
	if (lum >= 255) lum = 255;
	*output_data++ = (a << 8) | lum;
      }
    } else {
      for (auto i = 0; i < n; i++) {
	unsigned int input_offset = i * fd.getBytesPerPixel();
	unsigned char r = data[input_offset++] >> 4;
	unsigned char g = (fd.getBytesPerPixel() >= 1 ? data[input_offset++] : r) >> 4;
	unsigned char b = (fd.getBytesPerPixel() >= 2 ? data[input_offset++] : g) >> 4;
	unsigned char a = (fd.getBytesPerPixel() >= 3 ? data[input_offset++] : 0xff) >> 4;
#if defined __APPLE__ || defined __ANDROID__
	*output_data++ = (r << 12) | (g << 8) | (b << 4) | a;
#else
	*output_data++ = (b << 12) | (g << 8) | (r << 4) | a;
#endif
      }
    }
    
    return unique_ptr<ImageData>(new ImageData(tmp.get(), target_format, getWidth(), getHeight(), getLevels()));
  }
}

std::unique_ptr<ImageData>
ImageData::scale(unsigned int target_base_width, unsigned int target_base_height, unsigned int target_levels) const {
  auto & fd = getImageFormat(format);
  assert(!fd.getCompression());
  size_t target_size = calculateOffset(target_base_width, target_base_height, target_levels, format);
  // cerr << "scaling to " << target_base_width << " " << target_base_height << " " << target_levels << " => " << target_size << " bytes\n";
  std::unique_ptr<unsigned char[]> output_data(new unsigned char[target_size]);
  unsigned int target_width = target_base_width, target_height = target_base_height;
  stbir_resize_uint8(data.get(), getWidth(), getHeight(), 0, output_data.get(), target_width, target_height, 0, fd.getBytesPerPixel());

  if (target_levels > 1) {
    cerr << "creating mipmaps for format, channels = " << fd.getNumChannels() << ", bpp = " << fd.getBytesPerPixel() << endl;

    unsigned int source_width = target_width, source_height = target_height;
    target_width /= 2;
    target_height /= 2;

    unsigned int nch = fd.getBytesPerPixel();
    
    for (unsigned int level = 1; level < target_levels; level++) {
      unsigned char * source_data = output_data.get() + calculateOffset(target_base_width, target_base_height, level - 1, format);
      unsigned char * target_data = output_data.get() + calculateOffset(target_base_width, target_base_height, level, format);
      for (int y = 0; y < target_height; y++) {               
	for (int x = 0; x < target_width; x++) {
	  unsigned int source_offset = (2 * y * source_width + 2 * x) * nch;
	  unsigned int target_offset = (y * target_width + x) * nch;
	  for (unsigned int c = 0; c < nch; c++) {
	    target_data[target_offset] = ((int)source_data[source_offset]  + 
					  (int)source_data[source_offset + nch]  + 
					  (int)source_data[source_offset + nch + nch * source_width] +
					  (int)source_data[source_offset + nch * source_width]) / 4; 
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
  return unique_ptr<ImageData>(new ImageData(output_data.get(), getInternalFormat(), target_base_width, target_base_height, target_levels));
}
#endif
