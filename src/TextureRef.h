#ifndef _TEXTUREREF_H_
#define _TEXTUREREF_H_

#include "Texture.h"

namespace canvas {
  class TextureRef {
  public:
  TextureRef() : width(0), height(0), data(0) { }
  TextureRef(unsigned int _width, unsigned int _height, Texture * _data = 0) : width(_width), height(_height), data(_data) {
      if (data) data->incRefcnt();
    }
  TextureRef(const TextureRef & other)
    : width(other.width), height(other.height), data(other.data) {
      if (data) data->incRefcnt();
    }
    TextureRef & operator=(const TextureRef & other) {
      if (this != &other) {
	setData(other.data);
	width = other.width;
	height = other.height;
      }
      return *this;
    }
    ~TextureRef() {
      clear();
    }
  
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    unsigned int getTextureId() const { return data ? data->getTextureId() : 0; }
    bool isDefined() const { return data != 0; }

    void setWidth(unsigned int w) { width = w; }
    void setHeight(unsigned int h) { height = h; }
    void setData(canvas::Texture * _data) {
      if (_data != data) {
	if (data) clear();
	if (_data) {
	  data = _data;
	  data->incRefcnt();
	}
      }
    }
  
    void updateData(unsigned char * buffer) {
      if (data) data->updateData(buffer);
    }
  
    void clear() {
      if (data && data->decRefcnt() == 0) {
	delete data;
      }
      width = height = 0;
      data = 0;
    }

    const Texture * getData() const { return data; }

  private:
    unsigned int width, height;
    Texture * data;
  };
}

#endif
