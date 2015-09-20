#ifndef _HITREGION_H_
#define _HITREGION_H_

#include "Path.h"
#include <string>

namespace canvas {
  class HitRegion {
  public:
    HitRegion(const std::string & _id, const Path & _path, const std::string & _cursor)
      : id(_id), cursor(_cursor), path(_path) { }

    const std::string & getId() const { return id; }
    const std::string & getCursor() const { return cursor; }
    const Path & getPath() const { return path; }
    
  private:
    std::string id, cursor;
    Path path;
  };
};
#endif
