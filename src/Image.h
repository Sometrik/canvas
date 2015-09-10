#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstring>
#include <cassert>
#include <memory>

#include "ImageFormat.h"

namespace canvas {
  class Image {
  public:
  Image() : width(0), height(0), data(0), format(ImageFormat::UNDEF) { }
   Image(unsigned int _width, unsigned int _height, const unsigned char * _data, const ImageFormat & _format)
     : width(_width), height(_height), format(_format)
    {
      assert(_data);
      unsigned int s = width * height * format.getBytesPerPixel();
      data = new unsigned char[s];
      memcpy(data, _data, s);
    }
  Image(unsigned int _width, unsigned int _height, const ImageFormat & _format) : width(_width), height(_height), format(_format) {
      unsigned int s = width * height * format.getBytesPerPixel();
      data = new unsigned char[s];
      memset(data, 0, s);
    }
    Image(const Image & other)
      : width(other.getWidth()), height(other.getHeight()), format(other.format)
    {
      if (other.getData()) {
	size_t s = width * height * format.getBytesPerPixel();
	data = new unsigned char[s];
	memcpy(data, other.getData(), s);
      } else {
	data = 0;
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
	format = other.format;
	if (other.data) {
	  unsigned int s = width * height * format.getBytesPerPixel();
	  data = new unsigned char[s];
	  memcpy(data, other.data, s);
	} else {
	  data = 0;
	}
      }
      return *this;
    }

    std::shared_ptr<Image> changeFormat(const ImageFormat & target_format, unsigned int target_width = 0, unsigned int target_height = 0) const;
    std::shared_ptr<Image> scale(unsigned int target_width, unsigned int target_height) const;

    bool isValid() const { return width != 0 && height != 0; }
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    const ImageFormat & getFormat() const { return format; }
    const unsigned char * getData() const { return data; }

  protected:
    
  private:
    unsigned int width, height;
    unsigned char * data = 0;
    ImageFormat format;
    static bool etc1_initialized;
  };
};
#endif
