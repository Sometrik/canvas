#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "FilterMode.h"
#include "InternalFormat.h"

namespace canvas {
  class ImageData;
  
  class Texture {
  public:
    friend class TextureRef;

  Texture(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, FilterMode _min_filter, FilterMode _mag_filter, InternalFormat _internal_format, unsigned int _mipmap_levels)
    : logical_width(_logical_width), logical_height(_logical_height),
      actual_width(_actual_width), actual_height(_actual_height),
      mipmap_levels(_mipmap_levels),
      min_filter(_min_filter), mag_filter(_mag_filter),
      internal_format(_internal_format) { }     
    virtual ~Texture() { }

    virtual void updateData(const ImageData & image, unsigned int x, unsigned int y) = 0;
    virtual void generateMipmaps() { }
    virtual unsigned int getTextureId() const { return 0; }

    unsigned int getLogicalWidth() const { return logical_width; }
    unsigned int getLogicalHeight() const { return logical_height; }
    unsigned int getActualWidth() const { return actual_width; }
    unsigned int getActualHeight() const { return actual_height; }
    unsigned int getMipmapLevels() const { return mipmap_levels; }
    FilterMode getMinFilter() const { return min_filter; }
    FilterMode getMagFilter() const { return mag_filter; }
    InternalFormat getInternalFormat() const { return internal_format; }
    bool isDefined() const { return getTextureId() != 0; }

    int getUpdateCursor() const { return update_cursor; }
    void setUpdateCursor(int c) { update_cursor = c; }
    
  private:
    Texture(const Texture & other);
    Texture & operator=(const Texture & other);

    unsigned int logical_width, logical_height, actual_width, actual_height, mipmap_levels;
    FilterMode min_filter;
    FilterMode mag_filter;
    InternalFormat internal_format;
    int update_cursor = 0;
  };

  class TextureFactory {
  public:
    TextureFactory() { }
    
  };
};

#endif
