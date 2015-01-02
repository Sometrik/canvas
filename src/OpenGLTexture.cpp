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
  assert(buffer);

  if (!texture_id) {
    loaded_width = loaded_height = 0;
    if (!freed_textures.empty()) {
      texture_id = freed_textures.back();
      freed_textures.pop_back();
    } else {
      glGenTextures(1, &texture_id);
    }
    if (texture_id) total_textures++;    
  }
  assert(texture_id);

  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // glGenerateMipmap(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  if (loaded_width != getWidth() || loaded_height != getHeight()) {
    loaded_width = getWidth();
    loaded_height = getHeight();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()) );
    // glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, getMinFilter() == LINEAR_MIPMAP_LINEAR ? GL_TRUE : GL_FALSE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
  } else {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getWidth(), getHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
  }

  glGenerateMipmap(GL_TEXTURE_2D);
  glPopAttrib();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void
OpenGLTexture::updateData(const void * buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  assert(buffer);

  if (!texture_id) {
    loaded_width = loaded_height = 0;
    if (!freed_textures.empty()) {
      texture_id = freed_textures.back();
      freed_textures.pop_back();
    } else {
      glGenTextures(1, &texture_id);
    }
    if (texture_id) total_textures++;    
  }
  assert(texture_id);
  
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // glGenerateMipmap(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  if (loaded_width != getWidth() || loaded_height != getHeight()) {
    loaded_width = getWidth();
    loaded_height = getHeight();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()) );
    // glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, getMinFilter() == LINEAR_MIPMAP_LINEAR ? GL_TRUE : GL_FALSE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, 0);
  }

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
  glGenerateMipmap(GL_TEXTURE_2D);

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
OpenGLTexture::createTexture(unsigned int width, unsigned int height, FilterMode min_filter, FilterMode mag_filter) {
  return TextureRef(width, height, new OpenGLTexture(width, height, min_filter, mag_filter));  
}
