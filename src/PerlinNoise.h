#ifndef _PERLINNOISE_H_
#define _PERLINNOISE_H_

#include "Surface.h"

// #define USE_FIXEDPOINT

namespace canvas {
  class PerlinNoise : public Surface {
  public:
    PerlinNoise(unsigned int width, unsigned int height, int octaves = 1, float alpha = 2, float beta = 2);
    ~PerlinNoise() {
      delete[] buffer;
    }

    unsigned char * lockMemory(bool write_access = false) {
      delete[] buffer;
      unsigned int w = getWidth(), h = getHeight();
      buffer = new unsigned char[w * h * 3];
      unsigned int offset = 0;
      for (unsigned int y = 0; y < h; y++) {
	for (unsigned int x = 0; x < w; x++) {
	  int f = int((evaluate(x, y, 0) + 0.7) / 1.4 * 255);
	  unsigned char v = f < 0 ? 0 : (f >= 255 ? 255 : (unsigned char)f);
	  buffer[offset++] = v;
	  buffer[offset++] = v;
	  buffer[offset++] = v;
	}
      }
      return buffer;
    }

    void releaseMemory() {
      delete[] buffer;
      buffer = 0;
    }

  protected:
    float evaluatePerlin(float x, float y, float z);
    float evaluate(float x, float y, float z);
    
  private:  
    unsigned char * buffer = 0;

    int octaves;
    float alpha, beta;
    int p[512];
#ifdef USE_FIXEDPOINT
    int * fadetbl;
    int xftable[16], yftable[16], zftable[16];
#endif
  };
};

#endif /* _PERLINNOISE_H_ */
