#include <Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace canvas;

bool
Image::decode(const unsigned char * buffer, size_t size) {
  int w, h, channels;
  auto img_buffer = stbi_load_from_memory(buffer, size, &w, &h, &channels, 4);
  if (!img_buffer) {
    cerr << "Image decoding failed: " << stbi_failure_reason() << endl;
    return false;
  }
  cerr << "Image.cpp: loaded image, size = " << size << ", b = " << (void*)img_buffer << ", w = " << w << ", h = " << h << ", ch = " << channels << endl;
  assert(w && h && channels);    

  InternalFormat format = channels == 4 ? RGBA8 : RGB8;

  size_t numPixels = w * h;
  unsigned int * storage = new unsigned int[numPixels];
  for (unsigned int i = 0; i < numPixels; i++) {
    unsigned char r = img_buffer[4 * i + 0];
    unsigned char g = img_buffer[4 * i + 1];
    unsigned char b = img_buffer[4 * i + 2];
    unsigned char a = img_buffer[4 * i + 3];
    if (a) {
      storage[i] = (0xff * b / a) + ((0xff * g / a) << 8) + ((0xff * r / a) << 16) + (a << 24);
    } else {
      storage[i] = 0;
    }
  }
  stbi_image_free(img_buffer);

  data = std::make_shared<ImageData>((unsigned char *)storage, format, w, h);
  
  delete[] storage;

  return true;
}

void
Image::loadFile() {
  assert(!filename.empty());
  int w, h, channels;
  cerr << "trying to load " << filename << endl;
  auto img_buffer = stbi_load(filename.c_str(), &w, &h, &channels, 4);
  assert(img_buffer);
  cerr << "Image.cpp: loaded image, filename = " << filename << ", b = " << (void*)img_buffer << ", w = " << w << ", h = " << h << ", ch = " << channels << endl;
  assert(w && h && channels);    

  InternalFormat format = channels == 4 ? RGBA8 : RGB8;

  size_t numPixels = w * h;
  unsigned int * storage = new unsigned int[numPixels];
  for (unsigned int i = 0; i < numPixels; i++) {
    unsigned char r = img_buffer[4 * i + 0];
    unsigned char g = img_buffer[4 * i + 1];
    unsigned char b = img_buffer[4 * i + 2];
    unsigned char a = img_buffer[4 * i + 3];
    if (a) {
      storage[i] = (0xff * b / a) + ((0xff * g / a) << 8) + ((0xff * r / a) << 16) + (a << 24);
    } else {
      storage[i] = 0;
    }
  }
  stbi_image_free(img_buffer);

  data = std::make_shared<ImageData>((unsigned char *)storage, format, w, h);

  delete[] storage;  
}
