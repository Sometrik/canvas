#ifndef _PACKEDIMAGEDATA_H_
#define _PACKEDIMAGEDATA_H_

#include <InternalFormat.h>

#include <memory>

namespace canvas {
  class ImageData;
  
  class PackedImageData {
  public:
    PackedImageData() : format(InternalFormat::NO_FORMAT), width(0), height(0), levels(0), quality(0) { }
    PackedImageData(InternalFormat _format, unsigned short _levels, const ImageData & input);
    PackedImageData(InternalFormat _format, unsigned short _width, unsigned short _height, unsigned short _levels, const unsigned char * input = 0);
  
    void setQuality(unsigned short _quality) { quality = _quality; }
    unsigned short getQuality() const { return quality; }
    
    unsigned short getWidth() const { return width; }
    unsigned short getHeight() const { return height; }
    unsigned short getLevels() const { return levels; }
    unsigned short getBytesPerRow() const { return getBytesPerRow(width, format); }
    unsigned short getBytesPerPixel() const { return getBytesPerPixel(format); }
    InternalFormat getInternalFormat() const { return format; }

    static unsigned short getBytesPerRow(unsigned short width, InternalFormat format) {
      unsigned short bpp = getBytesPerPixel(format);
#ifdef __APPLE__
      return (bpp * width + 63) & ~63;
#else
      return bpp * width;
#endif
    }
    
    static unsigned short getBytesPerPixel(InternalFormat format) {
      switch (format) {
      case InternalFormat::NO_FORMAT: return 0;
      case InternalFormat::R8: return 1;
      case InternalFormat::RG8: return 2;
      case InternalFormat::RGB565: return 2;
      case InternalFormat::RGBA4: return 2;
      case InternalFormat::RGBA8: return 4;
      case InternalFormat::RGB8: return 4;
      case InternalFormat::RED_RGTC1: return 0; // n/a
      case InternalFormat::RG_RGTC2: return 0; // n/a
      case InternalFormat::RGB_DXT1: return 0; // n/a
      case InternalFormat::RGBA_DXT5: return 0; // n/a
      case InternalFormat::RGB_ETC1: return 0; // n/a
      case InternalFormat::LUMINANCE_ALPHA: return 2;
      case InternalFormat::LA44: return 1; // not a real OpenGL format
      case InternalFormat::R32F: return 4;
      case InternalFormat::RGB555: return 2;
      case InternalFormat::RGBA5551: return 2;
      }
      return 0;
    }

    static size_t calculateOffset(unsigned short width, unsigned short height, unsigned short level, InternalFormat format) {
      size_t s = 0;
      if (format == InternalFormat::RGB_ETC1 || format == InternalFormat::RGB_DXT1 || format == InternalFormat::RED_RGTC1) {
	for (unsigned int l = 0; l < level; l++) {
	  s += 8 * ((width + 3) / 4) * ((height + 3) / 4);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      } else if (format == InternalFormat::RG_RGTC2) {
	for (unsigned int l = 0; l < level; l++) {
	  s += 16 * ((width + 3) / 4) * ((height + 3) / 4);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      } else {
	for (unsigned int l = 0; l < level; l++) {
	  s += height * getBytesPerRow(width, format);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      }
      return s;
    }
    
    size_t calculateOffset(unsigned short level) const {
      return calculateOffset(width, height, level, format);
    }

    static size_t calculateSize(unsigned short width, unsigned short height, unsigned short levels, InternalFormat format) {
      return calculateOffset(width, height, levels, format);
    }
    
    size_t calculateSize() const {
      return calculateOffset(width, height, levels, format);
    }
    size_t calculateSizeForFirstLevel() const {
      return calculateOffset(width, height, 1, format);
    }

    const unsigned char * getData() const { return data.get(); }
    const unsigned char * getDataForLevel(unsigned short level) {
      return data.get() + calculateOffset(level);
    }

  private:
    InternalFormat format;
    unsigned short width, height, levels;
    unsigned short quality;
    std::unique_ptr<unsigned char[]> data;
    
    static bool etc1_initialized;
  };
};

#endif
