#ifndef _INTERNALFORMAT_H_
#define _INTERNALFORMAT_H_

namespace canvas {
  enum InternalFormat {
    R8 = 1,
    RG8,
    RGB565,
    RGBA4,
    RGBA8,
    RGB8,
    RGB8_24,
    RED_RGTC1,
    RG_RGTC2,
    RGB_DXT1,
    RGBA_DXT5,
    RGB_ETC1,
    LUMINANCE_ALPHA,
    LA44, // not a real OpenGL format
    R32F
  };
};

#endif
