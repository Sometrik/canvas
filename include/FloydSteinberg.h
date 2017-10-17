#ifndef _FLOYDSTEINBERG_H_
#define _FLOYDSTEINBERG_H_

#include <InternalFormat.h>
#include <memory>

namespace canvas {
  class ImageData;

  class FloydSteinberg  {
  public:
    FloydSteinberg(InternalFormat _target_format) : target_format(_target_format) { }

    std::unique_ptr<unsigned char[]> apply(const ImageData & input_image) const;

  private:
    InternalFormat target_format;
  };
};

#endif
