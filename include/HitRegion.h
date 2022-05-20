#ifndef _HITREGION_H_
#define _HITREGION_H_

#include "Path2D.h"

#include <string>
#include <functional>

namespace canvas {
  class HitRegion {
  public:
    HitRegion() { }
    HitRegion(const std::string & id, const Path2D & path, const std::string & cursor)
      : id_(id), cursor_(cursor), path_(path) { }
    HitRegion(std::function<std::string()> callback) : callback_(callback) { }

    const std::string & getId() const { return id_; }
    const std::string & getCursor() const { return cursor_; }
    const Path2D & getPath() const { return path_; }
    std::function<std::string()> & getCallback() { return callback_; }
    
  private:
    std::string id_, cursor_;
    Path2D path_;
    std::function<std::string()> callback_;
  };
};
#endif
