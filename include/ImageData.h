#ifndef _IMAGEDATA_H_
#define _IMAGEDATA_H_

#include <Color.h>

#include <cstring>
#include <memory>

namespace canvas {
  class ImageData {
  public:
    static ImageData nullImage;

  ImageData() : width(0), height(0), num_channels(0) { }
  ImageData(const unsigned char * _data, unsigned int _width, unsigned int _height, unsigned int _num_channels)
    : width(_width), height(_height), num_channels(_num_channels)
    {
      size_t s = calculateSize();
      data = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
      if (!_data) {
	memset(data.get(), 0, s);
      } else {
	memcpy(data.get(), _data, s);
      }
    }
  ImageData(unsigned int _width, unsigned int _height, unsigned int _num_channels)
    : width(_width), height(_height), num_channels(_num_channels) {
      size_t s = calculateSize();
      data = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
      memset(data.get(), 0, s);  
    }

    ImageData(const ImageData & other)
      : width(other.getWidth()), height(other.getHeight()), num_channels(other.num_channels)
    {
      size_t s = calculateSize();
      data = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
      if (other.getData()) {
	memcpy(data.get(), other.getData(), s);
      } else {
	memset(data.get(), 0, s);
      }
    }

    ImageData & operator=(const ImageData & other) = delete;
    
    std::unique_ptr<ImageData> scale(unsigned int target_width, unsigned int target_height) const;
    std::unique_ptr<ImageData> colorize(const Color & color) const;

    bool isValid() const { return width != 0 && height != 0 && num_channels != 0; }
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    unsigned int getNumChannels() const { return num_channels; }

    unsigned char * getData() { return data.get(); }
    const unsigned char * getData() const { return data.get(); }
    
    static size_t calculateSize(unsigned int width, unsigned int height, unsigned int num_channels) { return width * height * num_channels; }
    size_t calculateSize() const { return calculateSize(width, height, num_channels); }
    
  private:
    unsigned int width, height, num_channels;
    std::unique_ptr<unsigned char[]> data;
  };
};
#endif
