#ifndef _TEXTUREREF_H_
#define _TEXTUREREF_H_

#include "Texture.h"

namespace canvas {
  class TextureRef {
  public:
  TextureRef() : logical_width(0), logical_height(0), actual_width(0), actual_height(0), data(0) { }
  TextureRef(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, Texture * _data = 0)
    : logical_width(_logical_width), logical_height(_logical_height),
      actual_width(_actual_width), actual_height(_actual_height),
      data(_data) {
      if (data) data->incRefcnt();
    }
  TextureRef(const TextureRef & other)
    : logical_width(other.logical_width),
      logical_height(other.logical_height),
      actual_width(other.actual_width),
      actual_height(other.actual_height), data(other.data) {
      if (data) data->incRefcnt();
    }
    TextureRef & operator=(const TextureRef & other) {
      if (this != &other) {
	setData(other.data);
	logical_width = other.logical_width;
	logical_height = other.logical_height;
        actual_width = other.actual_width;
        actual_height = other.actual_height;
      }
      return *this;
    }
    ~TextureRef() {
      clear();
    }
  
    unsigned int getLogicalWidth() const { return logical_width; }
    unsigned int getLogicalHeight() const { return logical_height; }
    unsigned int getActualWidth() const { return actual_width; }
    unsigned int getActualHeight() const { return actual_height; }
    unsigned int getTextureId() const { return data ? data->getTextureId() : 0; }
    Texture * get() const { return data; }

    void setLogicalWidth(unsigned int w) { logical_width = w; }
    void setLogicalHeight(unsigned int h) { logical_height = h; }
    void setActualWidth(unsigned int w) { actual_width = w; }
    void setActualHeight(unsigned int h) { actual_height = h; }
    void setData(canvas::Texture * _data) {
      if (_data != data) {
	if (data) clear();
	if (_data) {
	  data = _data;
	  data->incRefcnt();
	}
      }
    }
  
    void updateData(const Image & image, unsigned int x, unsigned int y) {
      if (data) data->updateData(image, x, y);
    }
  
    void clear() {
      if (data && data->decRefcnt() <= 0) delete data;
      logical_width = logical_height = actual_width = actual_height = 0;
      data = 0;
    }

    const Texture * getData() const { return data; }

  private:
    unsigned int logical_width, logical_height, actual_width, actual_height;
    Texture * data;
  };
}

#endif
