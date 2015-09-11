#ifndef _INTERNALFORMAT_H_
#define _INTERNALFORMAT_H_

namespace canvas {
  enum InternalFormat {
    RG8 = 1,
    RGB565,
    RGBA4,
    RGBA8,
    // RG_RGTC,
    RGB_DXT1,
    RGBA_DXT5,
    RGB_ETC1,
    LUMINANCE_ALPHA
  };
};

#endif
