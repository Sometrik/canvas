#ifndef _IMAGEFORMAT_H_
#define _IMAGEFORMAT_H_

#define RGBA_TO_RED(v) (v & 0xff)
#define RGBA_TO_GREEN(v) ((v >> 8) & 0xff)
#define RGBA_TO_BLUE(v) ((v >> 16) & 0xff)
#define RGBA_TO_ALPHA(v) (v >> 24)

#define BGRA_TO_RED(v) ((v >> 16) & 0xff)
#define BGRA_TO_GREEN(v) ((v >> 8) & 0xff)
#define BGRA_TO_BLUE(v) (v & 0xff)
#define BGRA_TO_ALPHA(v) (v >> 24)

#define RGB565_TO_RED(v) ((v >> 11) * 255 / 31)
#define RGB565_TO_GREEN(v) (((v >> 5) & 0x3f) * 255 / 63)
#define RGB565_TO_BLUE(v) ((v & 0x1f) * 255 / 31)

#define PACK_RGB565(r, g, b) (r + (g << 5) + (b << 11))
#define PACK_RGB24(r, g, b) (r + (g << 8) + (b << 16))
#define PACK_RGBA32(r, g, b, a) (r + (g << 8) + (b << 16) + (a << 24))

namespace canvas {
  class ImageFormat {
  public:
    static ImageFormat UNDEF;
    static ImageFormat RGB24;
    static ImageFormat RGB32;
    static ImageFormat RGBA32;
    static ImageFormat RGB565;
    static ImageFormat PAL8;
    static ImageFormat LUM8;
    static ImageFormat ALPHA8;
    static ImageFormat LUMALPHA8;

    ImageFormat(unsigned short channels, unsigned short bytes_per_pixel, bool force_alpha = false);

    bool operator==(const ImageFormat & other) const {
      return channels == other.channels && bytes_per_pixel == other.bytes_per_pixel && force_alpha == other.force_alpha;
    }
    
    unsigned short getNumChannels() const { return channels; }
    unsigned short getBytesPerPixel() const { return bytes_per_pixel; }

    void setBytesPerPixel(unsigned short _bytes_per_pixel) { bytes_per_pixel = _bytes_per_pixel; }

    void clear() { channels = bytes_per_pixel = 0; }

    bool hasAlpha() const { return channels >= 4 || force_alpha; }
  
  private:
    unsigned short channels;
    unsigned short bytes_per_pixel;
    bool force_alpha;
  };
};

#endif
