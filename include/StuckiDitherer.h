#ifndef _STUCKIDITHERER_H_
#define _STUCKIDITHERER_H_

#include <Ditherer.h>

namespace canvas {
  class StuckiDitherer : public Ditherer {
  public:
    StuckiDitherer(const ImageData & input_image, InternalFormat _target_format)
      : Ditherer(input_image, _target_format) { }

    std::unique_ptr<ImageData> apply() override;
  };
};

#endif
