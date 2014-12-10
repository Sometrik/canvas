#include "PerlinSurface.h"

using namespace std;
using namespace canvas;

#include <cmath>

#define FIXED_PERMUTATION
#define FIXEDBITS 14

#ifdef FIXED_PERMUTATION
static unsigned char permutation[] = { 151,160,137,91,90,15,
				       131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
				       190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
				       88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
				       77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
				       102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
				       135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
				       5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
				       223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
				       129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
				       251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
				       49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
				       138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};
#endif

static inline float fade(float t) {
  return t * t * t * (t * (t * 6 - 15) + 10);
}

#ifdef USE_FIXEDPOINT

static inline int grad(int hashval, int x, int y, int z) {
  // CONVERT LO 4 BITS OF HASH CODE INTO 12 GRADIENT DIRECTIONS.
  int h = hashval & 15;
#if 1
#if 1
  int u = h < 8 ? x : y;
#else
  int u = h & 8 ? y : x;
#endif
#if 1
  int v = h < 4 ? y : h == 12 || h == 14 ? x : z;
#else 
  int v = h & 12 ? (h == 12 || h == 14 ? x : z) : y;
#endif
#if 1
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
#else
  if (h & 1) u *= -1;
  if (h & 2) v *= -1;
  return u + v;
#endif
#else
  int xf = 0, yf = 0, zf = 0;
  if (h & 8) xf = h & 1 ? -1 : 1;
  else yf = h & 1 ? -1 : 1;
  if (h & 12) {
    if (h == 12 || h == 14) xf += h & 2 ? -1 : 1;
    else z = hf & 2 ? -1 : 1;
  }
  else yf += h & 2 ? -1 : 1;
  return xf * x + yf * y + zf * z;
#endif
}

static inline int lerp(int t, int a, int b) {
  return a + ((t * (b - a)) >> FIXEDBITS);
}

#else

static inline float grad(int hashval, float x, float y, float z) {
  // CONVERT LO 4 BITS OF HASH CODE INTO 12 GRADIENT DIRECTIONS.
  int h = hashval & 15;
  float u = h < 8 ? x : y;
  float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static inline float lerp(float t, float a, float b) {
  return a + t * (b - a);
}

#endif

PerlinSurface::PerlinSurface(unsigned int _width, unsigned int _height, int _octaves, float _alpha, float _beta)
  : Surface(_width, _height), octaves(_octaves), alpha(_alpha), beta(_beta) 
{  
  
#ifndef FIXED_PERMUTATION
  int i, permutation[256];
  for (i = 0; i < 256; i++) {
    permutation[i] = i;
  }
  for (i = 0; i < 256; i++) {
    int tmp = permutation[i];
    int j = rand_r(&seed) & 255;
    permutation[i] = permutation[j];
    permutation[j] = tmp;
  }
#endif
  for(int i = 0; i < 256 ; i++) {
    p[256 + i] = p[i] = permutation[i]; 
  }
#ifdef USE_FIXEDPOINT
  int n = 1 << FIXEDBITS;
  fadetbl = new int[n];
  for (int i = 0; i < n; i++) {
    fadetbl[i] = (int)(fade((float)i / n) * n);
  }
  for (int h = 0; h < 15; h++) {
    int xf = 0, yf = 0, zf = 0;
    if (h & 8) xf = h & 1 ? -1 : 1;
    else yf = h & 1 ? -1 : 1;
    if (h & 12) {
      if (h == 12 || h == 14) xf += h & 2 ? -1 : 1;
      else zf = h & 2 ? -1 : 1;
    }
    else yf += h & 2 ? -1 : 1;
    xftable[h] = xf;
    yftable[h] = yf;
    zftable[h] = zf;
  }
#endif
}

float
PerlinSurface::evaluatePerlin(float x, float y, float z) {
#ifdef USE_FIXEDPOINT
  int b = 1 << FIXEDBITS;
  int bm = b - 1;
  float fpf = 1.0 / b;
#endif  
#ifdef USE_FIXEDPOINT
#if 1
  int x = int(vec[i].x * b), y = int(vec[i].y * b), z = int(vec[i].z * b);
#else
  int x = 0, y = 0, z = 0;
#endif
#if 1
  int flx = x >> FIXEDBITS, fly = y >> FIXEDBITS, flz = z >> FIXEDBITS;
  int X = flx & 255, Y = fly & 255, Z = flz & 255;
  x = x & bm;
  y = y & bm;
  z = z & bm;
  int u = fadetbl[x], v = fadetbl[y], w = fadetbl[z];
  int A = p[X    ] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
  int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
#else 
  int u = 0, v = 0, w = 0;
  int A = 0, B = 0, AA = 0, AB = 0, BA = 0, BB = 0;
#endif
  int x2 = x - b, y2 = y - b, z2 = z - b;
#if 1
  int g1 = grad(p[AA  ], x,   y,  z  );
  int g2 = grad(p[BA  ], x2,  y,  z  );
  int g3 = grad(p[AB  ], x,   y2, z  );
  int g4 = grad(p[BB  ], x2,  y2, z  );
  int g5 = grad(p[AA+1], x,   y,  z2 );
  int g6 = grad(p[BA+1], x2,  y,  z2 );
  int g7 = grad(p[AB+1], x,   y2, z2 );
  int g8 = grad(p[BB+1], x2,  y2, z2 );
#else
  int h;
  h = p[AA  ] & 15; int g1 = xftable[h] * x  + yftable[h] * y  + zftable[h] * z;
  h = p[BA  ] & 15; int g2 = xftable[h] * x2 + yftable[h] * y  + zftable[h] * z;
  h = p[AB  ] & 15; int g3 = xftable[h] * x  + yftable[h] * y2 + zftable[h] * z;
  h = p[BB  ] & 15; int g4 = xftable[h] * x2 + yftable[h] * y2 + zftable[h] * z;
  h = p[AA+1] & 15; int g5 = xftable[h] * x  + yftable[h] * y  + zftable[h] * z2;
  h = p[BA+1] & 15; int g6 = xftable[h] * x2 + yftable[h] * y  + zftable[h] * z2;
  h = p[AB+1] & 15; int g7 = xftable[h] * x  + yftable[h] * y2 + zftable[h] * z2;
  h = p[BB+1] & 15; int g8 = xftable[h] * x2 + yftable[h] * y2 + zftable[h] * z2;
  // int g1 = 0, g2 = 0, g3 = 0, g4 = 0, g5 = 0, g6 = 0, g7 = 0, g8 = 0;
#endif
#if 1
  int val = lerp(w, lerp(v, lerp(u, g1, g2),
			 lerp(u, g3, g4)),
		 lerp(v, lerp(u, g5, g6),
		      lerp(u, g7, g8)));
#else 
  int val = 0;
#endif
  return fpf * val;
#else
  
  // Find the unit cube that contains the point
  // float x = vec[i].x, y = vec[i].y, z = vec[i].z;
  int flx = (int)floorf(x), fly = (int)floorf(y), flz = (int)floorf(z);
  int X = flx & 255, Y = fly & 255, Z = flz & 255;
  // Find relative x, y, z of point in cube
  x -= flx;
  y -= fly;
  z -= flz;
  // Compute fade curves for each of x, y, z
  float u = fade(x), v = fade(y), w = fade(z);
  // Hash coordinates of the 8 cube corners
  int A = p[X    ] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
  int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
  // And add blended results from 8 corners of the cube
  return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x,   y,   z  ),
			         grad(p[BA  ], x-1, y,   z  )),
		         lerp(u, grad(p[AB  ], x,   y-1, z  ),
			         grad(p[BB  ], x-1, y-1, z  ))),
	         lerp(v, lerp(u, grad(p[AA+1], x,   y,   z-1),
			         grad(p[BA+1], x-1, y,   z-1)),
		         lerp(u, grad(p[AB+1], x,   y-1, z-1),
			         grad(p[BB+1], x-1, y-1, z-1))));
#endif
}

float
PerlinSurface::evaluate(float x, float y, float z) {
  float scale = 1;
  float v = 0;
  for (int o = 0; o < octaves; o++) {
    v += evaluatePerlin(x, y, z) / scale;
    x *= beta;
    y *= beta;
    z *= beta;
    scale *= alpha;
  }
  return v;
}
