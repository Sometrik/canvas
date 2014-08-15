#include "Surface.h"

#include "OpenGLTexture.h"

#include "Color.h"

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
  
  for (row++; row <= r; row++) {
    kernel.push_back(int(exp(-row * row / sigma22) / sqrtSigmaPi2 / first_value));
  }
  return kernel;
}
  
void
Surface::gaussianBlur(float hradius, float vradius) {
  unsigned char * buffer = lockMemory(true);
  assert(buffer);

  unsigned char * tmp = new unsigned char[width * height * 4];
  memset(tmp, 0, width * height * 4);
  vector<int> hkernel = make_kernel(hradius), vkernel = make_kernel(vradius);
  unsigned short hsize = hkernel.size(), vsize = vkernel.size();
  int htotal = 0, vtotal = 0;
  for (vector<int>::iterator it = hkernel.begin(); it != hkernel.end(); it++) {
    htotal += *it;
  }
  for (vector<int>::iterator it = vkernel.begin(); it != vkernel.end(); it++) {
    vtotal += *it;
  }
  for (unsigned int row = 0; row < height; row++) {
    for (unsigned int col = 0; col + hsize < width; col++) {
      int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
      for (unsigned int i = 0; i < hsize; i++) {
	unsigned char * ptr = buffer + (row * width + col + i) * 4;
	c0 += *ptr++ * hkernel[i];
	c1 += *ptr++ * hkernel[i];
	c2 += *ptr++ * hkernel[i];
	c3 += *ptr++ * hkernel[i];
      }
      unsigned char * ptr = tmp + (row * width + col + hsize / 2) * 4;
      *ptr++ = (unsigned char)(c0 / htotal);
      *ptr++ = (unsigned char)(c1 / htotal);
      *ptr++ = (unsigned char)(c2 / htotal);
      *ptr++ = (unsigned char)(c3 / htotal);
    }
  }
  memset(buffer, 0, width * height * 4);
  for (unsigned int col = 0; col < width; col++) {
    for (unsigned int row = 0; row + vsize < height; row++) {
      int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
      for (unsigned int i = 0; i < vsize; i++) {
	unsigned char * ptr = tmp + ((row + i) * width + col) * 4;
	c0 += *ptr++ * vkernel[i];
	c1 += *ptr++ * vkernel[i];
	c2 += *ptr++ * vkernel[i];
	c3 += *ptr++ * vkernel[i];
      }
      unsigned char * ptr = buffer + ((row + vsize / 2) * width + col) * 4;
      *ptr++ = (unsigned char)(c0 / vtotal);
      *ptr++ = (unsigned char)(c1 / vtotal);
      *ptr++ = (unsigned char)(c2 / vtotal);
      *ptr++ = (unsigned char)(c3 / vtotal);
    }
  }
  delete[] tmp;

  releaseMemory();
}

void
Surface::colorize(const Color & color) {
  
}

const TextureRef &
Surface::updateTexture() {
  flush();

  // cerr << "updating texture, this = " << this << ", tex = " << texture.getData() << "\n";
#ifdef OPENGL
  if (!texture.isDefined()) {
    texture = OpenGLTexture::createTexture(getWidth(), getHeight(), min_filter, mag_filter);
  }
#endif

  unsigned char * buffer = lockMemory();
  assert(buffer);
  
  // remove premultiplied alpha
  unsigned char * buffer2 = new unsigned char[getWidth() * getHeight() * 4];
  for (unsigned int i = 0; i < getWidth() * getHeight(); i++) {
    unsigned int red = buffer[4 * i + 0];
    unsigned int green = buffer[4 * i + 1];
    unsigned int blue = buffer[4 * i + 2];
    unsigned int alpha = buffer[4 * i + 3];
    if (alpha >= 1) {
      buffer2[4 * i + 0] = (unsigned char)(255 * red / alpha);
      buffer2[4 * i + 1] = (unsigned char)(255 * green / alpha);
      buffer2[4 * i + 2] = (unsigned char)(255 * blue / alpha);
    } else {
      buffer2[4 * i + 0] = 0;
      buffer2[4 * i + 1] = 0;
      buffer2[4 * i + 2] = 0;
    }
    buffer2[4 * i + 3] = alpha;
  }

  texture.updateData(buffer2);
  
  delete[] buffer2;

  releaseMemory();
  
  return texture;
}
