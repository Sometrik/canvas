#include "OpenGLTexture.h"

#include "TextureRef.h"

#ifdef WIN32
#include <GL/glew.h>
#endif

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
OpenGLTexture::updateData(const void * buffer) {
  updateData(buffer, 0, 0, getWidth(), getHeight());
}

void
OpenGLTexture::updateData(const void * buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  assert(buffer);

  bool initialize = false;
  if (!texture_id) {
    initialize = true;
    glGenTextures(1, &texture_id);
    if (texture_id >= 1) total_textures++;    
  }
  assert(texture_id >= 1);
  
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // glGenerateMipmap(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  if (initialize) {
    glTexStorage2D(GL_TEXTURE_2D, getMipmapLevels(), GL_RGBA8, getWidth(), getHeight());

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()) );
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, 0);
  }

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
  if (getMinFilter() == LINEAR_MIPMAP_LINEAR) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
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
OpenGLTexture::createTexture(unsigned int width, unsigned int height, FilterMode min_filter, FilterMode mag_filter, unsigned int mipmap_levels) {
  return TextureRef(width, height, new OpenGLTexture(width, height, min_filter, mag_filter, mipmap_levels));  
}
