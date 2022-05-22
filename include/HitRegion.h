#ifndef _HITREGION_H_
#define _HITREGION_H_

#include "Path2D.h"

#include <string>
#include <functional>

namespace canvas {
  class HitRegion {
  public:
    HitRegion() { }
    HitRegion(std::string id, Path2D path, std::string cursor)
      : id_(std::move(id)), path_(std::move(path)), cursor_(std::move(cursor)), callback_([]() -> std::string { return ""; }) { }
    HitRegion(std::string id, Path2D path, std::string cursor, std::function<std::string()> callback)
      : id_(std::move(id)), path_(std::move(path)), cursor_(std::move(cursor)), callback_(std::move(callback)) { }

    const std::string & getId() const { return id_; }
    const Path2D & getPath() const { return path_; }
    const std::string & getCursor() const { return cursor_; }
    std::function<std::string()> & getCallback() { return callback_; }
    
  private:
    std::string id_;
    Path2D path_;
    std::string cursor_;
    std::function<std::string()> callback_;
  };
};

#endif
