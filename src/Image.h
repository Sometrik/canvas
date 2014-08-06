#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstring>

namespace canvas {
  class Image {
  public:
    Image() 
      : width(0), height(0), data(0) {

    }
  Image(unsigned int _width, unsigned int _height, const unsigned char * _data)
    : width(_width), height(_height)
    {
      data = new unsigned char[width * height * 3];
      memcpy(data, _data, width * height * 3);
    }
    ~Image() {
      delete[] data;
    }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    const unsigned char * getData() const { return data; }

  private:
    unsigned int width, height;
    unsigned char * data;
  };
};
#endif
