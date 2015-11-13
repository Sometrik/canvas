#include "ImageFormat.h"

using namespace canvas;

ImageFormat ImageFormat::UNDEF(0, 0);
ImageFormat ImageFormat::RGB24(3, 3);
ImageFormat ImageFormat::RGB32(3, 4);
ImageFormat ImageFormat::RGBA32(4, 4);
ImageFormat ImageFormat::RGB565(3, 2);
ImageFormat ImageFormat::PAL8(3, 1);
ImageFormat ImageFormat::LUM8(1, 1);
ImageFormat ImageFormat::ALPHA8(1, 1, true);
ImageFormat ImageFormat::LUMALPHA8(2, 2, true);
ImageFormat ImageFormat::RGB_ETC1(3, 0, false, ImageFormat::ETC1);
ImageFormat ImageFormat::RGB_DXT1(3, 0, false, ImageFormat::DXT1);
ImageFormat ImageFormat::RGBA_DXT5(4, 0, false, ImageFormat::DXT5);
ImageFormat ImageFormat::FLOAT32(1, 4, false);
