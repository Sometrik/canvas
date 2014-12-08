#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstring>
#include <cassert>

namespace canvas {
  class Image {
  public:
    Image() : width(0), height(0), data(0), has_alpha(false) { }
    Image(unsigned int _width, unsigned int _height, const unsigned char * _data, bool _has_alpha)
      : width(_width), height(_height), has_alpha(_has_alpha)
    {
      assert(_data);
      unsigned int s = width * height * (has_alpha ? 4 : 3);
      data = new unsigned char[s];
      memcpy(data, _data, s);
    }
    Image(const Image & other)
      : width(other.getWidth()), height(other.getHeight()), has_alpha(other.hasAlpha())
    {
      if (other.getData()) {
	unsigned int s = width * height * (has_alpha ? 4 : 3);
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
	has_alpha = other.has_alpha;
	if (other.data) {
	  unsigned int s = width * height * (has_alpha ? 4 : 3);
	  data = new unsigned char[s];
	  memcpy(data, other.data, s);
	} else {
	  data = 0;
	}
      }
      return *this;
    }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    bool hasAlpha() const { return has_alpha; }
    const unsigned char * getData() const { return data; }

  private:
    unsigned int width, height;
    unsigned char * data;
    bool has_alpha;
  };
};
#endif
