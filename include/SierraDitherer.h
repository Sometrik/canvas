#ifndef _SIERRADITHERER_H_
#define _SIERRADITHERER_H_

#include <Ditherer.h>

namespace canvas {
  class SierraDitherer : public Ditherer {
  public:
    SierraDitherer(const ImageData & input_image, InternalFormat _target_format)
      : Ditherer(input_image, _target_format) { }

    std::unique_ptr<ImageData> apply() override;
  };
};

#endif
