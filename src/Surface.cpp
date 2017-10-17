#include <Surface.h>

#include <ImageData.h>

#include <cstring>
#include <vector>
#include <cmath>
#include <cassert>

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
Surface::blur(float hradius, float vradius) {
  if (!(hradius > 0 || vradius > 0)) {
    return;
  }

  unsigned char * buffer = (unsigned char *)lockMemory(true);
  assert(buffer);
  if (!buffer) {
    return;
  }

  if (num_channels == 4) {
    unsigned char * tmp = new unsigned char[actual_width * actual_height * 4];
    if (hradius > 0.0f) {
      vector<int> hkernel = make_kernel(hradius);
      unsigned short hsize = hkernel.size();

      int htotal = 0;
      for (auto & a : hkernel) htotal += a;

      memset(tmp, 0, actual_width * actual_height * 4);
      for (unsigned int row = 0; row < actual_height; row++) {
	for (unsigned int col = 0; col + hsize <= actual_width; col++) {
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
      for (auto & a : vkernel) vtotal += a;
      
      memset(buffer, 0, actual_width * actual_height * 4);
      for (unsigned int col = 0; col < actual_width; col++) {
	for (unsigned int row = 0; row + vsize <= actual_height; row++) {
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
  } else if (num_channels == 1) {
    unsigned char * tmp = new unsigned char[actual_width * actual_height];
    if (hradius > 0.0f) {
      vector<int> hkernel = make_kernel(hradius);
      unsigned short hsize = hkernel.size();

      int htotal = 0;
      for (auto & a : hkernel) htotal += a;

      memset(tmp, 0, actual_width * actual_height);
      for (unsigned int row = 0; row < actual_height; row++) {
	for (unsigned int col = 0; col + hsize <= actual_width; col++) {
	  int c0 = 0;
	  for (unsigned int i = 0; i < hsize; i++) {
	    unsigned char * ptr = buffer + (row * actual_width + col + i);
	    c0 += *ptr * hkernel[i];
	  }
	  unsigned char * ptr = tmp + (row * actual_width + col + hsize / 2);
	  *ptr = (unsigned char)(c0 / htotal);	  
	}
      }
    } else {
      memcpy(tmp, buffer, actual_width * actual_height);
    }
    if (vradius > 0) {
      vector<int> vkernel = make_kernel(vradius);
      unsigned short vsize = vkernel.size();
      int vtotal = 0;
      for (auto & a : vkernel) vtotal += a;
      
      memset(buffer, 0, actual_width * actual_height);
      for (unsigned int col = 0; col < actual_width; col++) {
	for (unsigned int row = 0; row + vsize <= actual_height; row++) {
	  int c0 = 0;
	  for (unsigned int i = 0; i < vsize; i++) {
	    unsigned char * ptr = tmp + ((row + i) * actual_width + col);
	    c0 += *ptr * vkernel[i];
	  }
	  unsigned char * ptr = buffer + ((row + vsize / 2) * actual_width + col);
	  *ptr = (unsigned char)(c0 / vtotal);
	}
      }
    } else {
      memcpy(buffer, tmp, actual_width * actual_height);
    }
    delete[] tmp;    
  }
  releaseMemory();
}
