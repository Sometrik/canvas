#include <OpenGLTexture.h>

#include <Image.h>
#include <Surface.h>

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

#ifdef __ANDROID__
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

#if defined __APPLE__ || defined __ANDROID__
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
bool OpenGLTexture::has_tex_storage = false;

struct format_description_s {
  GLenum internalFormat;
  GLenum format;
  GLenum type;
};

OpenGLTexture::OpenGLTexture(Surface & surface)
  : Texture(surface.getLogicalWidth(), surface.getLogicalHeight(), surface.getActualWidth(), surface.getActualHeight(), surface.getMinFilter(), surface.getMagFilter(), surface.getTargetFormat(), 1) {
  assert(getInternalFormat());
  assert(getLogicalWidth() > 0);
  assert(getLogicalHeight() > 0);
  assert(getActualWidth() > 0);
  assert(getActualHeight() > 0);
  auto image = surface.createImage(1.0f);
  updateData(image->getData(), 0, 0);
}

OpenGLTexture::OpenGLTexture(unsigned int _logical_width, unsigned int _logical_height, const ImageData & image)
  : Texture(_logical_width, _logical_height,
	    image.getWidth(), image.getHeight(),
	    NEAREST, NEAREST, image.getInternalFormat(), 1)
{
  assert(getInternalFormat());
  assert(getLogicalWidth() > 0);
  assert(getLogicalHeight() > 0);
  assert(getActualWidth() > 0);
  assert(getActualHeight() > 0);
  updateData(image, 0, 0);
}

static format_description_s getFormatDescription(InternalFormat internal_format) {
  switch (internal_format) {
  case NO_FORMAT: return { 0, 0, 0 };
  case R8: return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
  case RG8: return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
  case RGB565: return { GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 };
  case RGBA4: return { GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 };
  case RGBA8: 
#if defined __APPLE__ || defined __ANDROID__
    return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
#elif defined _WIN32
    return { GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV };
#else
    // Linux (doesn't work for OpenGL ES)
    return { GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV };
#endif
  case RGB8:
#if defined __APPLE__ || defined __ANDROID__
    return { GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE };
#elif defined _WIN32
    return { GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV };
#else
    // Linux
    return { GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV };
#endif
    // case RGB8_24: return GL_RGBA8;
  case RED_RGTC1: return { GL_COMPRESSED_RED_RGTC1, GL_RG, 0 };
  case RG_RGTC2: return { GL_COMPRESSED_RG_RGTC2, GL_RG, 0 };
  case RGB_ETC1: return { GL_COMPRESSED_RGB8_ETC2, GL_RGB, 0 };
  case RGB_DXT1: return { GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_RGB, 0 };
  case RGBA_DXT5: return { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, 0 };
  case LUMINANCE_ALPHA: return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
  case LA44: // pack luminance and alpha to single byte
    return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
  case R32F: return { GL_R32F, GL_RED, GL_FLOAT };
  default:
    break;
  }
  cerr << "unhandled format: " << int(internal_format) << endl;
  assert(0);
  return { 0, 0, 0 };
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
OpenGLTexture::updateTextureData(const ImageData & image, unsigned int x, unsigned int y) {
  unsigned int offset = 0;
  unsigned int current_width = image.getWidth(), current_height = image.getHeight();
  auto fd = getFormatDescription(getInternalFormat());
  bool filled = false;

  for (unsigned int level = 0; level < image.getLevels(); level++) {
    size_t size = image.calculateOffset(level + 1) - image.calculateOffset(level);
    // cerr << "plain tex: f = " << int(getInternalFormat()) << ", x = " << x << ", y = " << y << ", l = " << (level+1) << "/" << image.getLevels() << ", w = " << current_width << ", h = " << current_height << ", size = " << size << ", offset = " << offset << endl;
    assert(image.getData());

    if (fd.type == 0) { // compressed
      if (hasTexStorage() || is_data_initialized) {
	glCompressedTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, fd.internalFormat, (GLsizei)size, image.getData() + offset);
      } else {
	glCompressedTexImage2D(GL_TEXTURE_2D, level, fd.internalFormat, current_width, current_height, 0, (GLsizei)size, image.getData() + offset);
      }      
    } else if (hasTexStorage() || is_data_initialized) {
      glTexSubImage2D(GL_TEXTURE_2D, level, x, y, current_width, current_height, fd.format, fd.type, image.getData() + offset);
    } else {
      glTexImage2D(GL_TEXTURE_2D, level, fd.internalFormat, current_width, current_height, 0, fd.format, fd.type, image.getData() + offset);
      filled = true;
    }
    
    offset += size;
    current_width /= 2;
    current_height /= 2;
    x /= 2;
    y /= 2;
  }

  if (filled) is_data_initialized = true;
}

void
OpenGLTexture::updateData(const ImageData & image, unsigned int x, unsigned int y) {
  if (!global_init) {
    global_init = true;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }

  releaseTextures();

  bool initialize = false;
  if (!texture_id) {
    initialize = true;
    glGenTextures(1, &texture_id);
    // cerr << "created texture id " << texture_id << " (total = " << total_textures << ")" << endl;
    if (texture_id >= 1) total_textures++;    
  }
  assert(texture_id >= 1);

  glBindTexture(GL_TEXTURE_2D, texture_id);

  bool has_mipmaps = getMinFilter() == LINEAR_MIPMAP_LINEAR;
  if (initialize) {
    if (hasTexStorage()) {
      auto fd = getFormatDescription(getInternalFormat());
      glTexStorage2D(GL_TEXTURE_2D, has_mipmaps ? getMipmapLevels() : 1, fd.internalFormat, getActualWidth(), getActualHeight());
    }
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()));

    if (x != 0 || y != 0 || image.getWidth() != getActualWidth() || image.getHeight() != getActualHeight()) {
      int levels = has_mipmaps ? getMipmapLevels() : 1;
      ImageData img(getInternalFormat(), getActualWidth(), getActualHeight(), levels);
      updateTextureData(img, 0, 0);	
    }
  }
  
  if (image.getInternalFormat() == getInternalFormat() ||
      (image.getInternalFormat() == RGB8 && getInternalFormat() == RGBA8)) {
    updateTextureData(image, x, y);
  } else {
    if (getInternalFormat() != LA44) {
      cerr << "OpenGLTexture: doing online image conversion from " << int(image.getInternalFormat()) << " to " << int(getInternalFormat()) << " (SLOW)\n";
    }
    auto tmp_image = image.convert(getInternalFormat());
    updateTextureData(*tmp_image, x, y);
  }

  // if the image has only one level, and mipmaps are needed, generate them
  if (has_mipmaps && image.getLevels() == 1) {
    need_mipmaps = true;
  }
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
    for (auto id : freed_textures) {
      // cerr << "deleting texture " << id << endl;
      glDeleteTextures(1, &id);
    }
    freed_textures.clear();
  }
}

std::unique_ptr<Texture>
OpenGLTexture::createTexture(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, FilterMode min_filter, FilterMode mag_filter, InternalFormat _internal_format, unsigned int mipmap_levels) {
  assert(_internal_format);
  return std::unique_ptr<Texture>(new OpenGLTexture(_logical_width, _logical_height, _actual_width, _actual_height, min_filter, mag_filter, _internal_format, mipmap_levels));
}

std::unique_ptr<Texture>
OpenGLTexture::createTexture(Surface & surface) {
  return std::unique_ptr<Texture>(new OpenGLTexture(surface));
}

std::unique_ptr<Texture>
OpenGLTexture::createTexture(Image & image) {
  auto & data = image.getData();
  unsigned int lw = (unsigned int)(data.getWidth() * image.getDisplayScale());
  unsigned int lh = (unsigned int)(data.getHeight() * image.getDisplayScale());
  return std::unique_ptr<Texture>(new OpenGLTexture(lw, lh, data));
}
