#ifndef _FLOYDSTEINBERG_H_
#define _FLOYDSTEINBERG_H_

#include <Ditherer.h>

namespace canvas {
  class FloydSteinberg : public Ditherer {
  public:
    FloydSteinberg(const ImageData & input_image, InternalFormat _target_format)
      : Ditherer(input_image, _target_format) { }

    std::unique_ptr<ImageData> apply() override;
  };
};

#endif
