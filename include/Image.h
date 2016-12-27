#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <ImageData.h>

#include <string>

namespace canvas {
  class Image {
  public:
    Image() { }
    Image(const char * _filename) : filename(_filename) { }
    
    bool decode(const unsigned char * buffer, size_t size);

    const ImageData & getData() {
      if (!filename.empty() && !data.get()) {
	loadFile();
      }
      if (data.get()) {
	return *data;
      } else {
	return null_data;
      }
    }

  protected:
    void loadFile();
    
  private:
    std::string filename;
    std::shared_ptr<ImageData> data;

    static ImageData null_data;
  };
};
#endif
