#include <Texture.h>

#include <cassert>

using namespace canvas;

int
Texture::decRefcnt() {
  assert(refcnt >= 1);
  return --refcnt;
}
