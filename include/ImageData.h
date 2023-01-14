#ifndef _IMAGEDATA_H_
#define _IMAGEDATA_H_

#include <Color.h>

#include <cstring>
#include <memory>

namespace canvas {
  class ImageData {
  public:
    static ImageData nullImage;

    ImageData() : width_(0), height_(0), num_channels_(0) { }
    ImageData(const unsigned char * data, unsigned short width, unsigned short height, unsigned short num_channels)
      : width_(width), height_(height), num_channels_(num_channels)
    {
      auto s = calculateSize();
      data_ = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
      if (data) {
	memcpy(data_.get(), data, s);
      } else {
	memset(data_.get(), 0, s);
      }
    }
    ImageData(unsigned short width, unsigned short height, unsigned short num_channels)
      : width_(width), height_(height), num_channels_(num_channels) {
      auto s = calculateSize();
      data_ = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
      memset(data_.get(), 0, s);  
    }
    
    ImageData(const ImageData & other)
      : width_(other.getWidth()), height_(other.getHeight()), num_channels_(other.num_channels_)
    {
      auto s = calculateSize();
      data_ = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
      if (other.getData()) {
	memcpy(data_.get(), other.getData(), s);
      } else {
	memset(data_.get(), 0, s);
      }
    }

    ImageData & operator=(const ImageData & other) {
      if (this != &other) {
	width_ = other.width_;
	height_ = other.height_;
	num_channels_ = other.num_channels_;
	auto s = calculateSize();
	data_ = std::unique_ptr<unsigned char[]>(new unsigned char[s]);
	if (other.getData()) {
	  memcpy(data_.get(), other.getData(), s);
	} else {
	  memset(data_.get(), 0, s);
	}	
      }
      return *this;
    }

    ImageData(ImageData && other) = default;
    ImageData & operator=(ImageData && other) = default;
    
    std::unique_ptr<ImageData> scale(unsigned short target_width, unsigned short target_height) const;
    std::unique_ptr<ImageData> colorize(const Color & color) const;
    std::unique_ptr<ImageData> blur(float hradius, float vradius) const;

    void multiplyAlpha();
    
    bool isValid() const { return width_ != 0 && height_ != 0 && num_channels_ != 0; }
    unsigned short getWidth() const { return width_; }
    unsigned short getHeight() const { return height_; }
    unsigned short getNumChannels() const { return num_channels_; }

    unsigned char * getData() { return data_.get(); }
    const unsigned char * getData() const { return data_.get(); }

    unsigned short getBytesPerRow() const { return num_channels_ * width_; }
    
    static size_t calculateSize(unsigned short width, unsigned short height, unsigned short num_channels) { return width * height * num_channels; }
    size_t calculateSize() const { return calculateSize(width_, height_, num_channels_); }
    
  private:
    unsigned short width_, height_, num_channels_;
    std::unique_ptr<unsigned char[]> data_;
  };
};
#endif
