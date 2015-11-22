#include "Surface.h"

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

// standard deviation, number of boxes
static vector<int> boxesForGauss(float sigma, unsigned int n) {
  // Ideal averaging filter width 
  float wIdeal = sqrtf((12.0f * sigma * sigma / n) + 1);
  int wl = int(floor(wIdeal));
  if (wl % 2 == 0) wl--;
  int wu = wl + 2;
  
  float mIdeal = (12.0f * sigma * sigma - n*wl*wl - 4*n*wl - 3*n) / (-4*wl - 4);
  int m = int(round(mIdeal));
  // var sigmaActual = Math.sqrt( (m*wl*wl + (n-m)*wu*wu - n)/12 );

  vector<int> sizes;
  for (int i = 0; i < n; i++) sizes.push_back(i < m ? wl : wu);
  return sizes;
}

static void boxBlurH_4(unsigned char * scl, unsigned char * tcl, int w, int h, int r) {
  float iarr = 1 / (r+r+1);
  for (int ch = 0; ch < 4; ch++) {
    for (int i=0; i < h; i++) {
      int ti = i*w;
      int li = ti;
      int ri = ti + r;
      int fv = scl[4 * ti + ch];
      int lv = scl[4 * (ti + w - 1) + ch];
      int val = (r+1)*fv;
      for (int j = 0; j  < r; j++) val += scl[4 * (ti + j) + ch];
      for (int j = 0; j <= r; j++) {
	val += scl[4 * ri++ + ch] - fv;
	tcl[4 * ti++ + ch] = int(round(val * iarr));
      }
      for (int j = r+1; j < w-r; j++) {
	val += scl[4 * ri++ + ch] - scl[4 * li++ + ch];
	tcl[4 * ti++ + ch] = int(round(val*iarr));
      }
      for (int j = w-r; j < w; j++) {
	val += lv - scl[4 * li++ + ch];
	tcl[4 * ti++ + ch] = int(round(val*iarr));
      }
    }
  }
}

static void boxBlurT_4(unsigned char * scl, unsigned char * tcl, int w, int h, int r) {
  float iarr = 1 / (r+r+1);
  for (int ch = 0; ch < 4; ch++) {
    for (int i = 0; i < w; i++) {
      int ti = i;
      int li = ti;
      int ri = ti + r * w;
      int fv = scl[4 * ti + ch];
      int lv = scl[4 * (ti+w*(h-1)) + ch];
      int val = (r+1)*fv;
      for (int j = 0; j < r; j++) val += scl[4 * (ti+j*w) + ch];
      for (int j = 0; j <= r; j++) {
	val += scl[4 * ri + ch] - fv;
	tcl[4 * ti + ch] = int(round(val*iarr));
	ri += w;
	ti += w;
      }
      for (int j = r+1; j<h-r; j++) {
	val += scl[4 * ri + ch] - scl[4 * li + ch];
	tcl[4 * ti + ch] = int(round(val*iarr));
	li += w;
	ri += w;
	ti += w;
      }
      for (int j = h-r; j < h; j++) {
	val += lv - scl[4 * li + ch];
	tcl[4 * ti + ch] = int(round(val*iarr));
	li += w;
	ti += w;
      }
    }
  }
}

static void boxBlur_4(unsigned char * scl, unsigned char * tcl, int w, int h, int r) {
  for (int i = 0; i < 4 * w * h; i++) tcl[i] = scl[i];
  boxBlurH_4(tcl, scl, w, h, r);
  boxBlurT_4(scl, tcl, w, h, r);
}

void
Surface::blur(float r) {
  auto bxs = boxesForGauss(r, 3);

  int w = getActualWidth(), h = getActualHeight();
  unsigned char * scl = (unsigned char *)lockMemory(true);  
  unsigned char * tcl = new unsigned char[w * h * 4];

  cerr << "boxes: " << bxs[0] << ", " << bxs[1] << ", " << bxs[2] << endl;
  
  boxBlur_4(scl, tcl, w, h, (bxs[0]-1) / 2);
  boxBlur_4(tcl, scl, w, h, (bxs[1]-1) / 2);
  boxBlur_4(scl, tcl, w, h, (bxs[2]-1) / 2);

  memcpy(scl, tcl, w * h * 4);
  delete[] tcl;
  releaseMemory();
}

void
Surface::slowBlur(float hradius, float vradius, float alpha) {
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

std::shared_ptr<Image>
Surface::createImage() {
  unsigned char * buffer = (unsigned char *)lockMemory(false);
  assert(buffer);
  shared_ptr<Image> image(new Image(buffer, hasAlpha() ? ImageFormat::RGBA32 : ImageFormat::RGB32, getActualWidth(), getActualHeight()));
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
  if (size >= 4 && buffer[0] == 0x89 && buffer[1] == 0x50 && buffer[2] == 0x4e && buffer[3] == 0x47) {
    return true;
  } else {
    return false;
  }
}

bool
Surface::isJPEG(const unsigned char * buffer, size_t size) {
  if (size >= 3 && buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff) {
    return true;
  } else {
    return false;
  }
}

bool
Surface::isGIF(const unsigned char * buffer, size_t size) {
  if (size >= 6 && buffer[0] == 'G' && buffer[1] == 'I' && buffer[2] == 'F' && buffer[3] == '8' && (buffer[4] == '7' || buffer[4] == '9') && buffer[5] == 'a') {
    return true;
  } else {
    return false;
  }
}
