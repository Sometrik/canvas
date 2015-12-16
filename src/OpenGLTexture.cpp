#include "OpenGLTexture.h"

#include "TextureRef.h"
#include "Image.h"
#include "Surface.h"

#define GL_GLEXT_PROTOTYPES

#if defined __APPLE__
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif (defined GL_ES)
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#else
#define GL_GLEXT_PROTOTYPES

#ifdef _WIN32
#include <GL/glew.h>
#include <windows.h>
#endif

#ifdef ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GL/gl.h>

#ifdef _WIN32
#include "glext.h"
#else
#include <GL/glext.h>
#endif
#endif

#endif

#if defined __APPLE__ || defined ANDROID
#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0
#endif
#ifndef GL_COMPRESSED_RED_RGTC1
#define GL_COMPRESSED_RED_RGTC1 0x8DBB
#endif
#ifndef GL_COMPRESSED_RG_RGTC2
#define GL_COMPRESSED_RG_RGTC2 0x8DBD
#endif
#endif

#include <cassert>
#include <iostream>

using namespace std;
using namespace canvas;

size_t OpenGLTexture::total_textures = 0;
vector<unsigned int> OpenGLTexture::freed_textures;
bool OpenGLTexture::global_init = false;

OpenGLTexture::OpenGLTexture(Surface & surface)
  : Texture(surface.getLogicalWidth(), surface.getLogicalHeight(), surface.getActualWidth(), surface.getActualHeight(), surface.getMinFilter(), surface.getMagFilter(), surface.getTargetFormat(), 1) {
  assert(getInternalFormat());
  auto image = surface.createImage();
  updateData(*image, 0, 0);
}

static GLenum getOpenGLInternalFormat(InternalFormat internal_format) {
  switch (internal_format) {
  case R8: return GL_R8;
  case RG8: return GL_RG8;
  case RGB565: return GL_RGB565;
  case RGBA4: return GL_RGBA4;
  case RGBA8: return GL_RGBA8;
  case RGB8: return GL_RGB8;
  case RGB8_24: return GL_RGBA8;
  case RED_RGTC1: return GL_COMPRESSED_RED_RGTC1;
  case RG_RGTC2: return GL_COMPRESSED_RG_RGTC2;
  case RGB_ETC1: return GL_COMPRESSED_RGB8_ETC2;
  case RGB_DXT1: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
  case RGBA_DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
  case LUMINANCE_ALPHA: return GL_RG8;
  case LA44: return GL_R8; // pack luminance and alpha to single byte
  case R32F: return GL_R32F;
    
  }
  assert(0);
  return 0;
}

static GLenum getOpenGLFilterType(FilterMode mode) {
  switch (mode) {
  case NEAREST: return GL_NEAREST;
  case LINEAR: return GL_LINEAR;
  case LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
  }
  return 0;
}

void
OpenGLTexture::updateCompressedData(const Image & image, unsigned int x, unsigned int y) {
  unsigned int offset = 0;
  unsigned int current_width = image.getWidth(), current_height = image.getHeight();
  GLenum format = getOpenGLInternalFormat(getInternalFormat());
  for (unsigned int level = 0; level < image.getLevels(); level++) {
    size_t size = image.calculateOffset(level + 1) - image.calculateOffset(level);
    // cerr << "compressed tex: x = " << x << ", y = " << y << ", l = " << (level+1) << "/" << image.getLevels() << ", w = " << current_width << ", h = " << current_height << ", offset = " << offset << ", size = " << size << endl;
    glCompressedTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, format, size, image.getData() + offset);
    offset += size;
    current_width /= 2;
    current_height /= 2;
    x /= 2;
    y /= 2;
  }
}

void
OpenGLTexture::updatePlainData(const Image & image, unsigned int x, unsigned int y) {
  unsigned int offset = 0;
  unsigned int current_width = image.getWidth(), current_height = image.getHeight();
  GLenum format = getOpenGLInternalFormat(getInternalFormat());

  for (unsigned int level = 0; level < image.getLevels(); level++) {
    size_t size = image.calculateOffset(level + 1) - image.calculateOffset(level);
    // cerr << "plain tex: x = " << x << ", y = " << y << ", l = " << (level+1) << "/" << image.getLevels() << ", w = " << current_width << ", h = " << current_height << ", size = " << size << ", offset = " << offset << endl;

    if (getInternalFormat() == RGBA8) {
#if defined __APPLE__ || defined ANDROID 
      glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, GL_RGBA, GL_UNSIGNED_BYTE, image.getData() + offset);
#else
      glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, image.getData() + offset);    
      // glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image.getWidth(), height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image.getData());
#endif
    } else if (getInternalFormat() == LA44) {
      glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, GL_RED, GL_UNSIGNED_BYTE, image.getData() + offset);      
    } else if (getInternalFormat() == RGB565) {
      glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, image.getData() + offset);      
    } else if (getInternalFormat() == R32F) {
      glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, current_width, current_height, GL_RED, GL_FLOAT, image.getData() + offset);
    } else {
      cerr << "unhandled format " << int(getInternalFormat()) << endl;
      assert(0);
    }
    
    // glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, getOpenGLInternalFormat(getInternalFormat()), size, image.getData() + offset);
    
    offset += size;
    current_width /= 2;
    current_height /= 2;
    x /= 2;
    y /= 2;
  }
}

void
OpenGLTexture::updateData(const Image & image, unsigned int x, unsigned int y) {
  if (!global_init) {
    global_init = true;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }
    
  bool initialize = false;
  if (!texture_id) {
    initialize = true;
    glGenTextures(1, &texture_id);
    cerr << "created texture id " << texture_id << " (total = " << total_textures << ")" << endl;
    if (texture_id >= 1) total_textures++;    
  }
  assert(texture_id >= 1);

  glBindTexture(GL_TEXTURE_2D, texture_id);

  bool has_mipmaps = getMinFilter() == LINEAR_MIPMAP_LINEAR;
  if (initialize) {
    glTexStorage2D(GL_TEXTURE_2D, has_mipmaps ? getMipmapLevels() : 1, getOpenGLInternalFormat(getInternalFormat()), getActualWidth(), getActualHeight());
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()));

    if (x != 0 || y != 0 || image.getWidth() != getActualWidth() || image.getHeight() != getActualHeight()) {
      int levels = has_mipmaps ? getMipmapLevels() : 1;
      if (getInternalFormat() == RGB_ETC1) {
	Image img(ImageFormat::RGB_ETC1, getActualWidth(), getActualHeight(), levels);
	updateCompressedData(img, 0, 0);	
      } else if (getInternalFormat() == RGB_DXT1) {
	Image img(ImageFormat::RGB_DXT1, getActualWidth(), getActualHeight(), levels);
	updateCompressedData(img, 0, 0);	
      } else if (getInternalFormat() == RGBA8) {
	Image img(ImageFormat::RGBA32, getActualWidth(), getActualHeight(), levels);
	updatePlainData(img, 0, 0);
      } else if (getInternalFormat() == LA44) {
	Image img(ImageFormat::LA44, getActualWidth(), getActualHeight(), levels);
	updatePlainData(img, 0, 0);
      } else if (getInternalFormat() == RGB565) {
	Image img(ImageFormat::RGB565, getActualWidth(), getActualHeight(), levels);
	updatePlainData(img, 0, 0);
      } else if (getInternalFormat() == R32F) {
	assert(levels == 1);
	Image img(ImageFormat::FLOAT32, getActualWidth(), getActualHeight(), levels);
	updatePlainData(img, 0, 0);
      } else {
	assert(0);
      }
    }
  }

  if (getInternalFormat() == R32F) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, image.getWidth(), image.getHeight(), GL_RED, GL_FLOAT, image.getData());
  } else if (getInternalFormat() == R8) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, image.getWidth(), image.getHeight(), GL_RED, GL_UNSIGNED_BYTE, image.getData());
  } else if (getInternalFormat() == LA44) {
    auto tmp_image = image.convert(ImageFormat::LA44);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image->getWidth(), tmp_image->getHeight(), GL_RED, GL_UNSIGNED_BYTE, tmp_image->getData());
  } else if (getInternalFormat() == LUMINANCE_ALPHA) {
    if (image.getFormat() == ImageFormat::LA88) {
      glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, image.getWidth(), image.getHeight(), GL_RG, GL_UNSIGNED_BYTE, image.getData());
    } else {
      auto tmp_image = image.convert(ImageFormat::LA88);
      glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image->getWidth(), tmp_image->getHeight(), GL_RG, GL_UNSIGNED_BYTE, tmp_image->getData()); 
    }
  } else if (getInternalFormat() == RGB565) {
    auto tmp_image = image.convert(ImageFormat::RGB565);
    updatePlainData(*tmp_image, x, y);    
    // glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image->getWidth(), tmp_image->getHeight(), GL_RGB, GL_UNSIGNED_SHORT_5_6_5, tmp_image->getData());
  } else if (getInternalFormat() == RGBA4) {
    auto tmp_image = image.convert(ImageFormat::RGBA4);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image->getWidth(), tmp_image->getHeight(), GL_RGBA4, GL_UNSIGNED_SHORT_4_4_4_4, tmp_image->getData());
  } else if (getInternalFormat() == RGB_ETC1) {
    if (image.getFormat().getCompression() == ImageFormat::ETC1) {
      updateCompressedData(image, x, y);
    } else {
      cerr << "WARNING: compression should be done in thread\n";
      auto tmp_image = image.convert(ImageFormat::RGB_ETC1);
      updateCompressedData(*tmp_image, x, y);
    }
  } else if (getInternalFormat() == RGB_DXT1) {
    if (image.getFormat().getCompression() == ImageFormat::DXT1) {
      updateCompressedData(image, x, y);
    } else {
      cerr << "WARNING: compression should be done in thread\n";
      auto tmp_image = image.convert(ImageFormat::RGB_DXT1);
      updateCompressedData(*tmp_image, x, y);
    }    
  } else if (getInternalFormat() == RG_RGTC2) {
    if (image.getFormat().getCompression() == ImageFormat::RGTC2) {
      updateCompressedData(image, x, y);
    } else {
      cerr << "WARNING: compression should be done in thread\n";
      auto tmp_image = image.convert(ImageFormat::RG_RGTC2);
      updateCompressedData(*tmp_image, x, y);
    }    
  } else {
    updatePlainData(image, x, y);    
  }
    
  if (has_mipmaps && image.getLevels() == 1) {
    need_mipmaps = true;
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

void
OpenGLTexture::generateMipmaps() {
  if (need_mipmaps) {
    if (getInternalFormat() != RGB_DXT1 && getInternalFormat() != RGB_ETC1 && getInternalFormat() != LA44 && getInternalFormat() != RED_RGTC1 && getInternalFormat() != RG_RGTC2) {
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      cerr << "unable to generate mipmaps for compressed texture!\n";
    }
    need_mipmaps = false;
  }
}

void
OpenGLTexture::releaseTextures() {
  if (!freed_textures.empty()) {
    cerr << "DELETING TEXTURES: " << OpenGLTexture::getFreedTextures().size() << "/" << OpenGLTexture::getNumTextures() << endl;
    
    for (vector<unsigned int>::const_iterator it = freed_textures.begin(); it != freed_textures.end(); it++) {
      GLuint texid = *it;
      glDeleteTextures(1, &texid);
    }
    freed_textures.clear();
  }
}

TextureRef
OpenGLTexture::createTexture(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, FilterMode min_filter, FilterMode mag_filter, InternalFormat _internal_format, unsigned int mipmap_levels) {
  assert(_internal_format);
  return TextureRef(_logical_width, _logical_height, _actual_width, _actual_height, new OpenGLTexture(_logical_width, _logical_height, _actual_width, _actual_height, min_filter, mag_filter, _internal_format, mipmap_levels));
}

TextureRef
OpenGLTexture::createTexture(Surface & surface) {
  return TextureRef( surface.getLogicalWidth(),
		     surface.getLogicalHeight(),
		     surface.getActualWidth(),
		     surface.getActualHeight(),
		     new OpenGLTexture(surface)
		     );
}
