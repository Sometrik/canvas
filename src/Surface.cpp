#include "Surface.h"

#include "OpenGLTexture.h"

#include "Color.h"
#include "Image.h"

#include <cstring>
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>

using namespace std;
using namespace canvas;

static vector<int> make_kernel(float radius) {
  int r = (int)ceil(radius);
  int rows = 2 * r + 1;
  float sigma = radius / 3;
  float sigma22 = 2.0f * sigma * sigma;
  float sigmaPi2 = 2.0f * float(M_PI) * sigma;
  float sqrtSigmaPi2 = sqrt(sigmaPi2);
  // float radius2 = radius*radius;
  vector<int> kernel;
  kernel.reserve(rows);

  int row = -r;
  float first_value = exp(-row*row/sigma22) / sqrtSigmaPi2;
  kernel.push_back(1);
  
  for (unsigned int i = 1; i < rows; i++, row++) {
    // for (row++; row <= r; row++) {
    kernel.push_back(int(exp(-row * row / sigma22) / sqrtSigmaPi2 / first_value));
  }
  return kernel;
}
  
void
Surface::gaussianBlur(float hradius, float vradius, float alpha) {
  if (!(hradius > 0 || vradius > 0)) {
    return;
  }

  unsigned char * buffer = (unsigned char *)lockMemory(true);
  assert(buffer);

  unsigned char * tmp = new unsigned char[actual_width * actual_height * 4];
  if (hradius > 0.0f) {
    vector<int> hkernel = make_kernel(hradius);
    unsigned short hsize = hkernel.size();
    int htotal = 0;
    for (vector<int>::iterator it = hkernel.begin(); it != hkernel.end(); it++) {
      htotal += *it;
    }
    htotal = int(htotal / alpha);
    memset(tmp, 0, actual_width * actual_height * 4);
    for (unsigned int row = 0; row < actual_height; row++) {
      for (unsigned int col = 0; col + hsize < actual_width; col++) {
	int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
	for (unsigned int i = 0; i < hsize; i++) {
	  unsigned char * ptr = buffer + (row * actual_width + col + i) * 4;
	  c0 += *ptr++ * hkernel[i];
	  c1 += *ptr++ * hkernel[i];
	  c2 += *ptr++ * hkernel[i];
	  c3 += *ptr++ * hkernel[i];
	}
	unsigned char * ptr = tmp + (row * actual_width + col + hsize / 2) * 4;
	*ptr++ = (unsigned char)(c0 / htotal);
	*ptr++ = (unsigned char)(c1 / htotal);
	*ptr++ = (unsigned char)(c2 / htotal);
	*ptr++ = (unsigned char)(c3 / htotal);
      }
    }
  } else {
    memcpy(tmp, buffer, actual_width * actual_height * 4);
  }
  if (vradius > 0) {
    vector<int> vkernel = make_kernel(vradius);
    unsigned short vsize = vkernel.size();
    int vtotal = 0;
    for (vector<int>::iterator it = vkernel.begin(); it != vkernel.end(); it++) {
      vtotal += *it;
    }
    vtotal = int(vtotal / alpha);

    memset(buffer, 0, actual_width * actual_height * 4);
    for (unsigned int col = 0; col < actual_width; col++) {
      for (unsigned int row = 0; row + vsize < actual_height; row++) {
	int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
	for (unsigned int i = 0; i < vsize; i++) {
	  unsigned char * ptr = tmp + ((row + i) * actual_width + col) * 4;
	  c0 += *ptr++ * vkernel[i];
	  c1 += *ptr++ * vkernel[i];
	  c2 += *ptr++ * vkernel[i];
	  c3 += *ptr++ * vkernel[i];
	}
	unsigned char * ptr = buffer + ((row + vsize / 2) * actual_width + col) * 4;
	*ptr++ = (unsigned char)(c0 / vtotal);
	*ptr++ = (unsigned char)(c1 / vtotal);
	*ptr++ = (unsigned char)(c2 / vtotal);
	*ptr++ = (unsigned char)(c3 / vtotal);
      }
    }
  } else {
    memcpy(buffer, tmp, actual_width * actual_height * 4);
  }
  delete[] tmp;

  releaseMemory();
}

static inline unsigned char toByte(float f) {
  int a = int(f * 255);
  if (a < 0) a = 0;
  else if (a > 255) a = 255;
  return (unsigned char)a;
}

void
Surface::colorFill(const Color & color) {
  unsigned char * buffer = (unsigned char *)lockMemory(true);
  assert(buffer);
  unsigned int red = toByte(color.red * color.alpha);
  unsigned int green = toByte(color.green * color.alpha);
  unsigned int blue = toByte(color.blue * color.alpha);
  unsigned int alpha = toByte(color.alpha);
  for (unsigned int i = 0; i < actual_width * actual_height; i++) {
    unsigned char * ptr = buffer + (i * 4);
    unsigned int dest_alpha = ptr[3];
    *ptr++ = (unsigned char)(dest_alpha * red / 255);
    *ptr++ = (unsigned char)(dest_alpha * green / 255);
    *ptr++ = (unsigned char)(dest_alpha * blue / 255);
    *ptr++ = (unsigned char)(dest_alpha * alpha / 255);
  }
  releaseMemory();
}

void
Surface::multiply(const Color & color) {
  unsigned char * buffer = (unsigned char *)lockMemory(true);
  assert(buffer);
  unsigned int red = toByte(color.red);
  unsigned int green = toByte(color.green);
  unsigned int blue = toByte(color.blue);
  unsigned int alpha = toByte(color.alpha);
  for (unsigned int i = 0; i < actual_width * actual_height; i++) {
    unsigned char * ptr = buffer + (i * 4);
    ptr[0] = (unsigned char)(ptr[0] * red / 255);
    ptr[1] = (unsigned char)(ptr[1] * green / 255);
    ptr[2] = (unsigned char)(ptr[2] * blue / 255);
    ptr[3] = (unsigned char)(ptr[3] * alpha / 255);
  }
  releaseMemory();
}

const TextureRef &
Surface::updateTexture() {
  if (!texture.isDefined()) {
    texture = OpenGLTexture::createTexture(getLogicalWidth(), getLogicalHeight(), getActualWidth(), getActualHeight(), min_filter, mag_filter);
  }

  void * buffer = lockMemory();
  assert(buffer);
  texture.updateData(buffer);
  releaseMemory();
  
  return texture;
}

const TextureRef &
Surface::updateTexture(unsigned int x0, unsigned int y0, unsigned int subwidth, unsigned int subheight) {
  if (!texture.isDefined()) {
    texture = OpenGLTexture::createTexture(getLogicalWidth(), getLogicalHeight(), getActualWidth(), getActualHeight(), min_filter, mag_filter);
  }

  void * buffer = lockMemoryPartial(x0, y0, subwidth, subheight);
  texture.updateData(buffer, x0, y0, subwidth, subheight);
  
  releaseMemory();
  
  return texture;
}

std::shared_ptr<Image>
Surface::createImage(unsigned int required_width, unsigned int required_height) {
  unsigned char * buffer = (unsigned char *)lockMemory(false, required_width, required_height);
  assert(buffer);
  shared_ptr<Image> image(new Image(required_width ? required_width : getActualWidth(), required_height ? required_height : getActualHeight(), buffer, true));
  releaseMemory();
  
  return image;
}

void *
Surface::lockMemoryPartial(unsigned int x0, unsigned int y0, unsigned int required_width, unsigned int required_height) {
  unsigned int * buffer = (unsigned int *)lockMemory();
  assert(buffer);

  delete[] scaled_buffer;
  scaled_buffer = new unsigned int[required_width * required_height];

  unsigned int offset = 0;
  for (unsigned int y = y0; y < y0 + required_height; y++) {
    for (unsigned int x = x0; x < x0 + required_width; x++) {
      scaled_buffer[offset++] = buffer[y * actual_width + x];
    }
  }

  return scaled_buffer;
}

bool
Surface::isPNG(const unsigned char * buffer, size_t size) {
  if (size >= 4 && (buffer[0] == 0x89 || buffer[1] == 0x50 || buffer[2] == 0x4e || buffer[3] == 0x47)) {
    return true;
  } else {
    return false;
  }
}

bool
Surface::isJPEG(const unsigned char * buffer, size_t size) {
  if (size >= 3 && (buffer[0] == 0xff || buffer[1] == 0xd8 || buffer[2] == 0xff)) {
    return true;
  } else {
    return false;
  }
}
