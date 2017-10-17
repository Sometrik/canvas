#include <Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace canvas;

bool
Image::decode(const unsigned char * buffer, size_t size) {
  data = loadFromMemory(buffer, size);
  return data.get() != 0;
}

std::unique_ptr<ImageData>
Image::loadFromMemory(const unsigned char * buffer, size_t size) {
  int w, h, channels;
  auto img_buffer = stbi_load_from_memory(buffer, size, &w, &h, &channels, 0);
  if (!img_buffer) {
    cerr << "Image decoding failed: " << stbi_failure_reason() << endl;
    return std::unique_ptr<ImageData>(nullptr);
  }
  // cerr << "Image.cpp: loaded image, size = " << size << ", b = " << (void*)img_buffer << ", w = " << w << ", h = " << h << ", ch = " << channels << endl;
  assert(w && h && channels);    

  std::unique_ptr<ImageData> data = std::unique_ptr<ImageData>(new ImageData((unsigned char *)img_buffer, w, h, channels));
  
  stbi_image_free(img_buffer);
  
  return data;
}

std::unique_ptr<ImageData>
Image::loadFromFile(const std::string & filename) {
  assert(!filename.empty());
  int w, h, channels;
  cerr << "trying to load " << filename << endl;
  auto img_buffer = stbi_load(filename.c_str(), &w, &h, &channels, 0);
  if (!img_buffer) {
    cerr << "image loading failed\n";
    return std::unique_ptr<ImageData>(nullptr);
  }
  // cerr << "Image.cpp: loaded image, filename = " << filename << ", b = " << (void*)img_buffer << ", w = " << w << ", h = " << h << ", ch = " << channels << endl;
  assert(w && h && channels);    

  std::unique_ptr<ImageData> data = std::unique_ptr<ImageData>(new ImageData((unsigned char *)img_buffer, w, h, channels));
  
  stbi_image_free(img_buffer);

  return data;
}
