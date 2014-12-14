#include "OpenGLTexture.h"

#include "TextureRef.h"

#define GL_GLEXT_PROTOTYPES
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __WXMAC__
#include "OpenGL/gl.h"
#else
#include <GL/gl.h>
#endif

#ifdef _WIN32
#include "glext.h"
#else
#include <GL/glext.h>
#endif

#include <cassert>

using namespace std;
using namespace canvas;

size_t OpenGLTexture::total_textures = 0;
vector<unsigned int> OpenGLTexture::freed_textures;

static GLenum getOpenGLFilterType(FilterMode mode) {
  switch (mode) {
  case NEAREST: return GL_NEAREST;
  case LINEAR: return GL_LINEAR;
  case LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
  }
  return 0;
}

void
OpenGLTexture::updateData(unsigned char * buffer) {
  assert(buffer);

  // glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  if (!is_initialized) {
    is_initialized = true;
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()) );
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, getMinFilter() == LINEAR_MIPMAP_LINEAR ? GL_TRUE : GL_FALSE);
  }

  // glGenerateMipmap(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(),
	       0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
}

void
OpenGLTexture::updateData(unsigned char * buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  assert(buffer);
  
  // glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  if (!is_initialized) {
    is_initialized = true;
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()) );
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, getMinFilter() == LINEAR_MIPMAP_LINEAR ? GL_TRUE : GL_FALSE);
  }

  // glGenerateMipmap(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
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
OpenGLTexture::createTexture(unsigned int width, unsigned int height, FilterMode min_filter, FilterMode mag_filter) {
  if (freed_textures.empty()) {
    unsigned int id;
    glGenTextures(1, &id);
    return TextureRef(width, height, new OpenGLTexture(width, height, id, min_filter, mag_filter));
  } else {
    unsigned int id = freed_textures.back();
    freed_textures.pop_back();
    return TextureRef(width, height, new OpenGLTexture(width, height, id, min_filter, mag_filter));
  }
}
