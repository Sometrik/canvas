#ifndef _INTERNALFORMAT_H_
#define _INTERNALFORMAT_H_

namespace canvas {
  enum InternalFormat {
    RG8 = 1,
    RGB565,
    RGBA4,
    RGBA8,
    COMPRESSED_RG,
    COMPRESSED_RGB,
    COMPRESSED_RGBA,
    LUMINANCE_ALPHA
  };
};

#endif
