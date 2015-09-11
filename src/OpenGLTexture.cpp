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

#include <GL/gl.h>

#ifdef _WIN32
#include "glext.h"
#else
#include <GL/glext.h>
#endif

#endif

#include <cassert>
#include <iostream>

using namespace std;
using namespace canvas;

size_t OpenGLTexture::total_textures = 0;
vector<unsigned int> OpenGLTexture::freed_textures;

static bool checkGLError(const std::string & message) {
  GLenum errLast = GL_NO_ERROR;
  bool has_errors = false;
  for ( ;; ) {
    GLenum err = glGetError();
    if ( err == GL_NO_ERROR ) {
      break;
    }
    
    // normally the error is reset by the call to glGetError() but if
    // glGetError() itself returns an error, we risk looping forever here
    // so check that we get a different error than the last time
    if ( err == errLast ) {
      cerr << "OpenGL error state couldn't be reset.\n";
      break;
    }
    
    errLast = err;
    
    cerr << message << ": OpenGL error " << err << "\n";
    // assert(0);
    has_errors = true;
  }
  
  return has_errors;
}

static GLenum getOpenGLInternalFormat(InternalFormat internal_format) {
  switch (internal_format) {
  case RG8: return GL_RG8;
  case RGB565: return GL_RGB565;
  case RGBA4: return GL_RGBA4;
  case RGBA8: return GL_RGBA8;
    // case RG_RGTC: return GL_COMPRESSED_RG11_EAC;
  case RGB_ETC1: return GL_COMPRESSED_RGB8_ETC2;
  case RGB_DXT1: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
  case RGBA_DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
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
OpenGLTexture::updateData(const void * buffer) {
  updateData(buffer, 0, 0, getActualWidth(), getActualHeight());
}

void
OpenGLTexture::updateData(const void * buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  checkGLError("error before texture update");
  
  assert(buffer);

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
    Image tmp_image(width, height, (const unsigned char *)buffer, ImageFormat::RGBA32);
    auto tmp_image2 = tmp_image.changeFormat(ImageFormat::LUMALPHA8);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RG, GL_UNSIGNED_BYTE, tmp_image2->getData());
  } else if (getInternalFormat() == RGB565) {
    Image tmp_image(width, height, (const unsigned char *)buffer, ImageFormat::RGBA32);
    auto tmp_image2 = tmp_image.changeFormat(ImageFormat::RGB565);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, tmp_image2->getData());
  } else if (getInternalFormat() == RGBA4) {
    assert(0);
  } else if (getInternalFormat() == RGB_ETC1) {
    Image tmp_image(width, height, (const unsigned char *)buffer, ImageFormat::RGB32);
    auto tmp_image2 = tmp_image.changeFormat(ImageFormat::RGB_ETC1);
    unsigned int size = 8 * (tmp_image2->getWidth() / 4) * (tmp_image2->getHeight() / 4);
    cerr << "sending ETC1 texture to OpenGL, x = " << x << ", y = " << y << ", w = " << tmp_image2->getWidth() << ", h = " << tmp_image2->getHeight() << ", size = " << size << endl;
    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image2->getWidth(), tmp_image2->getHeight(), getOpenGLInternalFormat(getInternalFormat()), size, tmp_image2->getData());
  } else if (getInternalFormat() == RGB_DXT1) {
    Image tmp_image(width, height, (const unsigned char *)buffer, ImageFormat::RGB32);
    auto tmp_image2 = tmp_image.changeFormat(ImageFormat::RGB_DXT1);
    unsigned int size = 8 * (tmp_image2->getWidth() / 4) * (tmp_image2->getHeight() / 4);
    cerr << "sending DXT1 texture to OpenGL, x = " << x << ", y = " << y << ", w = " << tmp_image2->getWidth() << ", h = " << tmp_image2->getHeight() << ", size = " << size << endl;
    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image2->getWidth(), tmp_image2->getHeight(), getOpenGLInternalFormat(getInternalFormat()), size, tmp_image2->getData());
  } else if (getInternalFormat() == RGBA_DXT5) {
    Image tmp_image(width, height, (const unsigned char *)buffer, ImageFormat::RGB32);
    auto tmp_image2 = tmp_image.changeFormat(ImageFormat::RGBA_DXT5);
    unsigned int size = 16 * (tmp_image2->getWidth() / 4) * (tmp_image2->getHeight() / 4);
    cerr << "sending DXT5 texture to OpenGL, x = " << x << ", y = " << y << ", w = " << tmp_image2->getWidth() << ", h = " << tmp_image2->getHeight() << ", size = " << size << endl;
    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, x, y, tmp_image2->getWidth(), tmp_image2->getHeight(), getOpenGLInternalFormat(getInternalFormat()), size, tmp_image2->getData());
  } else {
#if defined __APPLE__
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
#else
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buffer);    
    // glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
#endif
  }
  if (has_mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  checkGLError("Failed to update texture");
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
