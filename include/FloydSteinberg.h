#ifndef _FLOYDSTEINBERG_H_
#define _FLOYDSTEINBERG_H_

#include <InternalFormat.h>
#include <ImageFormat.h>
#include <memory>

namespace canvas {
  class ImageData;

  class FloydSteinberg  {
  public:
    FloydSteinberg(InternalFormat _target_format) : target_format(_target_format) { }

    std::unique_ptr<ImageData> apply(const ImageData & input_image) const;

  private:
    InternalFormat target_format;
  };
};

#endif
