#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <ImageData.h>

#include <string>

namespace canvas {
  class Image {
  public:
    Image(float _display_scale) : display_scale(_display_scale) { }
    Image(const char * _filename, float _display_scale)
      : filename(_filename), display_scale(_display_scale) { }
    Image(const std::string & _filename, float _display_scale)
      : filename(_filename), display_scale(_display_scale) { }

    virtual ~Image() { }

    bool decode(const unsigned char * buffer, size_t size);
    void convert(InternalFormat target_format) {
      if (!filename.empty() && !data.get()) {
	loadFile();
      }
      if (data.get()) data = data->convert(target_format);
    }
    void scale(unsigned int target_width, unsigned int target_height, unsigned int target_levels = 1) {
      if (!filename.empty() && !data.get()) {
	loadFile();
      }
      if (data.get()) {
	unsigned int width = (unsigned int)(target_width * display_scale);
	unsigned int height = (unsigned int)(target_height * display_scale);
	data = data->scale(width, height, target_levels);
      }
    }

    const ImageData & getData() {
      if (!filename.empty() && !data.get()) {
	loadFile();
      }
      if (data.get()) {
	return *data;
      } else {
	return ImageData::nullImage;
      }
    }

#if 0
    std::shared_ptr<ImageData> & getDataPtr() {
      if (!filename.empty() && !data.get()) {
	loadFile();
      }
      return data;
    }
#endif

    std::string getFilename() const { return filename; }
    float getDisplayScale() const { return display_scale; }

  protected:
    static std::unique_ptr<ImageData> loadFromMemory(const unsigned char * buffer, size_t size);
    static std::unique_ptr<ImageData> loadFromFile(const std::string & filename);
    virtual void loadFile() {
      data = loadFromFile(filename);
      if (!data.get()) filename.clear();
    }
    
  private:
    std::string filename;
    std::unique_ptr<ImageData> data;
    float display_scale;
  };
};
#endif
