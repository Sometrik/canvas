#include "OpenGLTexture.h"

#include "TextureRef.h"
#include "Image.h"

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

#if 1
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

#ifdef __APPLE__
#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0
#endif
#endif

#include <cassert>
#include <iostream>

using namespace std;
using namespace canvas;

size_t OpenGLTexture::total_textures = 0;
vector<unsigned int> OpenGLTexture::freed_textures;

static bool flushErrors() {
  GLenum errLast = GL_NO_ERROR;
  bool has_errors = false;
  for ( ;; ) {
    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
      break;
    }
    if (err == errLast) {
      break;
    }
    errLast = err;
    
    cerr << "got error " << err << " before texture update" << endl;
    has_errors = true;
  }
  
  return has_errors;
}

static GLenum getLastError() {
  GLenum errLast = GL_NO_ERROR;
  for ( ;; ) {
    GLenum err = glGetError();
    if ( err == GL_NO_ERROR ) {
      break;
    }
    if ( err == errLast ) {
      break;
    }   
    errLast = err;
  }  
  return errLast;
}

static GLenum getOpenGLInternalFormat(InternalFormat internal_format) {
  switch (internal_format) {
  case RG8: return GL_RG8;
  case RGB565: return GL_RGB565;
  case RGBA4: return GL_RGBA4;
  case RGBA8: return GL_RGBA8;
    // case RG_RGTC: return GL_COMPRESSED_RG11_EAC;
  case RGB_ETC1: return GL_COMPRESSED_RGB8_ETC2;
#ifdef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
  case RGB_DXT1: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
  case RGBA_DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif
  case LUMINANCE_ALPHA: return GL_RG8;
  }
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
  for (unsigned int level = 0; level < image.getLevels(); level++) {
    size_t size = image.calculateOffset(level + 1) - image.calculateOffset(level);
    // cerr << "compressed tex: x = " << x << ", y = " << y << ", l = " << (level+1) << "/" << image.getLevels() << ", w = " << current_width << ", h = " << current_height << ", offset = " << offset << ", size = " << size << endl;
    glCompressedTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, getOpenGLInternalFormat(getInternalFormat()), size, image.getData() + offset);
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
  for (unsigned int level = 0; level < image.getLevels(); level++) {
    size_t size = image.calculateOffset(level + 1) - image.calculateOffset(level);
    // cerr << "plain tex: x = " << x << ", y = " << y << ", l = " << (level+1) << "/" << image.getLevels() << ", w = " << current_width << ", h = " << current_height << ", size = " << size << endl;
    
#ifndef GL_BGRA
    // defined __APPLE__
    glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, GL_RGBA, GL_UNSIGNED_BYTE, image.getData() + offset);
#else
    glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, image.getData() + offset);    
// glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image.getWidth(), height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image.getData());
#endif
    
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
  flushErrors();
    
  bool initialize = false;
  if (!texture_id) {
    initialize = true;
    glGenTextures(1, &texture_id);
    if (texture_id >= 1) total_textures++;    
  }
  assert(texture_id >= 1);
  
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  bool has_mipmaps = getMinFilter() == LINEAR_MIPMAP_LINEAR;
  if (initialize) {
#if 1
    glTexStorage2D(GL_TEXTURE_2D, has_mipmaps ? getMipmapLevels() : 1, getOpenGLInternalFormat(getInternalFormat()), getActualWidth(), getActualHeight());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
#endif
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()));
  }

  if (getInternalFormat() == LUMINANCE_ALPHA) {
    auto tmp_image = image.convert(ImageFormat::LUMALPHA8);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image->getWidth(), tmp_image->getHeight(), GL_RG, GL_UNSIGNED_BYTE, tmp_image->getData());
  } else if (getInternalFormat() == RGB565) {
    auto tmp_image = image.convert(ImageFormat::RGB565);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image->getWidth(), tmp_image->getHeight(), GL_RGB, GL_UNSIGNED_SHORT_5_6_5, tmp_image->getData());
  } else if (getInternalFormat() == RGBA4) {
    assert(0);
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
  } else {
    updatePlainData(image, x, y);    
  }
  if (has_mipmaps && getInternalFormat() != RGB_DXT1 && getInternalFormat() != RGB_ETC1 && image.getLevels() == 1) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  GLenum err = getLastError();
  if (err != GL_NO_ERROR) {
    cerr << "failed to update texture (err = " << int(err) << ", x = " << x << ", y = " << y << ", tw = " << getActualWidth() << ", th = " << getActualHeight() << ", w = " << image.getWidth() << ", h = " << image.getHeight() << ", l = " << image.getLevels() << ", c = " << int(image.getFormat().getCompression()) << ")" << endl;
  }
}

void
OpenGLTexture::releaseTextures() {
  // cerr << "DELETING TEXTURES: " << OpenGLTexture::getFreedTextures().size() << "/" << OpenGLTexture::getNumTextures() << endl;
  
  for (vector<unsigned int>::const_iterator it = freed_textures.begin(); it != freed_textures.end(); it++) {
    GLuint texid = *it;
    glDeleteTextures(1, &texid);
  }
  freed_textures.clear();
}

TextureRef
OpenGLTexture::createTexture(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, FilterMode min_filter, FilterMode mag_filter, InternalFormat _internal_format, unsigned int mipmap_levels) {
  return TextureRef(_logical_width, _logical_height, _actual_width, _actual_height, new OpenGLTexture(_logical_width, _logical_height, _actual_width, _actual_height, min_filter, mag_filter, _internal_format, mipmap_levels));
}
