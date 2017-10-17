#ifndef _PACKEDIMAGEDATA_H_
#define _PACKEDIMAGEDATA_H_

#include <InternalFormat.h>

#include <memory>
#include <cstring>

namespace canvas {
  class ImageData;
  
  class PackedImageData {
  public:
  PackedImageData() : format(NO_FORMAT), width(0), height(0), levels(0), quality(0) { }
    PackedImageData(InternalFormat _format, unsigned int _levels, const ImageData & input);
    PackedImageData(InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels);
  
    void setQuality(short _quality) { quality = _quality; }
    short getQuality() const { return quality; }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    InternalFormat getInternalFormat() const { return format; }

    static unsigned int getBytesPerPixel(InternalFormat format) {
      switch (format) {
      case NO_FORMAT: return 0;
      case R8: return 1;
      case RG8: return 2;
      case RGB565: return 2;
      case RGBA4: return 2;
      case RGBA8: return 4;
      case RGB8: return 4;
      case RED_RGTC1: return 0; // n/a
      case RG_RGTC2: return 0; // n/a
      case RGB_DXT1: return 0; // n/a
      case RGBA_DXT5: return 0; // n/a
      case RGB_ETC1: return 0; // n/a
      case LUMINANCE_ALPHA: return 2;
      case LA44: return 1; // not a real OpenGL format
      case R32F: return 4;
      case RGBA5551: return 2;
      }
      return 0;
    }

    static size_t calculateOffset(unsigned int width, unsigned int height, unsigned int level, InternalFormat format) {
      size_t s = 0;
      if (format == RGB_ETC1 || format == RGB_DXT1 || format == RED_RGTC1) {
	for (unsigned int l = 0; l < level; l++) {
	  s += 8 * ((width + 3) / 4) * ((height + 3) / 4);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      } else if (format == RG_RGTC2) {
	for (unsigned int l = 0; l < level; l++) {
	  s += 16 * ((width + 3) / 4) * ((height + 3) / 4);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      } else {
	for (unsigned int l = 0; l < level; l++) {
	  s += width * height * getBytesPerPixel(format);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      }
      return s;
    }
    size_t calculateOffset(unsigned int level) const {
      return calculateOffset(width, height, level, format);
    }
    static size_t calculateSize(unsigned int width, unsigned int height, unsigned int levels, InternalFormat format) { return calculateOffset(width, height, levels, format); }
    size_t calculateSize() const { return calculateOffset(width, height, levels, format); }

    const unsigned char * getData() const { return data.get(); }
    const unsigned char * getDataForLevel(unsigned int level) {
      return data.get() + calculateOffset(level);
    }

  private:
    InternalFormat format;
    unsigned int width, height, levels;
    short quality;
    std::unique_ptr<unsigned char[]> data;
    static bool etc1_initialized;
  };
};

#endif
