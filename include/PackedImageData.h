#ifndef _PACKEDIMAGEDATA_H_
#define _PACKEDIMAGEDATA_H_

#include <InternalFormat.h>

#include <memory>
#include <cstring>

namespace canvas {
  class PackedImageData {
  public:
  PackedImageData() : width(0), height(0), levels(0), quality(0), format(NO_FORMAT) { }

  PackedImageData(const unsigned char * _data, InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels = 1)
    : width(_width), height(_height), levels(_levels), format(_format)
    {
      size_t s = calculateSize();
      data = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
      if (!_data) {
	memset(data.get(), 0, s);
      } else {
	memcpy(data.get(), _data, s);
      }
    }

    void setQuality(short _quality) { quality = _quality; }
    short getQuality() const { return quality; }

        static inline const ImageFormat & getImageFormat(InternalFormat format) {
      switch (format) {
      case RGB_ETC1: return ImageFormat::RGB_ETC1;
      case RGB_DXT1: return ImageFormat::RGB_DXT1;
      case RED_RGTC1: return ImageFormat::RED_RGTC1;
      case RG_RGTC2: return ImageFormat::RG_RGTC2;
      case RGBA8: return ImageFormat::RGBA32;
      case RGB8: return ImageFormat::RGB32;
      case RGB8_24: return ImageFormat::RGB24;
      case R8: return ImageFormat::LUM8;
      case LA44: return ImageFormat::LA44;
      case RG8: case LUMINANCE_ALPHA: return ImageFormat::LA88;
      case R32F: return ImageFormat::FLOAT32;
      case RGB565: return ImageFormat::RGB565;
      case RGBA4: return ImageFormat::RGBA4;
      case RGBA_DXT5: return ImageFormat::RGBA_DXT5;
      case NO_FORMAT:
      case RGBA5551: // not yet supported
	return ImageFormat::UNDEF;
      }
      return ImageFormat::UNDEF;
    }

    static size_t calculateOffset(unsigned int width, unsigned int height, unsigned int level, InternalFormat input_format) {
      auto & format = getImageFormat(input_format);
      size_t s = 0;
      if (format.getCompression() == ImageFormat::ETC1 || format.getCompression() == ImageFormat::DXT1 || format.getCompression() == ImageFormat::RGTC1) {
	for (unsigned int l = 0; l < level; l++) {
	  s += 8 * ((width + 3) / 4) * ((height + 3) / 4);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      } else if (format.getCompression() == ImageFormat::RGTC2) {
	for (unsigned int l = 0; l < level; l++) {
	  s += 16 * ((width + 3) / 4) * ((height + 3) / 4);
	  width = (width + 1) / 2;
	  height = (height + 1) / 2;
	}
      } else {
	for (unsigned int l = 0; l < level; l++) {
	  s += width * height * format.getBytesPerPixel();
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

  private:
    unsigned int width, height, levels;
    short quality;
    std::unique_ptr<unsigned char[]> data;
    InternalFormat format;
  };
};

#endif
