#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstring>
#include <memory>

#include "ImageFormat.h"

namespace canvas {
  class Image {
  public:
  Image() : width(0), height(0), data(0), format(ImageFormat::UNDEF) { }
  Image(const unsigned char * _data, const ImageFormat & _format, unsigned int _width, unsigned int _height, unsigned int _levels = 1)
    : width(_width), height(_height), levels(_levels), format(_format)
    {
      size_t s = calculateSize();
      data = new unsigned char[s];
      if (!_data) {
	memset(data, 0, s);
      } else {
	memcpy(data, _data, s);
      }
    }
#if 0
  Image(const ImageFormat & _format, unsigned int _width, unsigned int _height, unsigned int _levels = 1) : width(_width), height(_height), levels(_levels), format(_format) {
      
    }
#endif
    Image(const Image & other)
      : width(other.getWidth()), height(other.getHeight()), levels(other.levels), format(other.format)     
    {
      size_t s = calculateSize();
      data = new unsigned char[s];
      if (other.getData()) {
	memcpy(data, other.getData(), s);
      } else {
	memset(data, 0, s);
      }
    }
    ~Image() {
      delete[] data;
    }

    Image & operator=(const Image & other) {
      if (&other != this) {
	delete[] data;
	width = other.width;
	height = other.height;
	levels = other.levels;
	format = other.format;
	size_t s = calculateSize();	
	data = new unsigned char[s];
	if (other.data) {
	  memcpy(data, other.data, s);
	} else {
	  memset(data, 0, s);
	}
      }
      return *this;
    }

    std::shared_ptr<Image> convert(const ImageFormat & target_format) const;
    std::shared_ptr<Image> scale(unsigned int target_width, unsigned int target_height, unsigned int target_levels = 1) const;
    std::shared_ptr<Image> createMipmaps(unsigned int levels) const;

    bool isValid() const { return width != 0 && height != 0; }
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    unsigned int getLevels() const { return levels; }
    const ImageFormat & getFormat() const { return format; }
    const unsigned char * getData() const { return data; }
    const unsigned char * getDataForLevel(unsigned int level) {
      return data + calculateOffset(level);
    }

    static size_t calculateOffset(unsigned int width, unsigned int height, unsigned int level, const ImageFormat & format) {
      size_t s = 0;
      if (format.getCompression() == ImageFormat::ETC1 || format.getCompression() == ImageFormat::DXT1) {
	for (unsigned int l = 0; l < level; l++) {
	  s += 8 * ((width + 3) / 4) * ((height + 3) / 4);
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
    static size_t calculateSize(unsigned int width, unsigned int height, unsigned int levels, const ImageFormat & format) { return calculateOffset(width, height, levels, format); }
    size_t calculateSize() const { return calculateOffset(width, height, levels, format); }

  protected:
    
  private:
    unsigned int width, height, levels;
    unsigned char * data = 0;
    ImageFormat format;
    static bool etc1_initialized;
  };
};
#endif
