#ifndef IMG_TGA_IMAGE_HH__
#define IMG_TGA_IMAGE_HH__

#include <stdint.h>
#include <vector>
#include <assert.h>
#include <string.h>

#include "color.hh"

namespace img {

// Force to align to 1
#pragma pack(push, 1)

/*
 * Header format:
 * |++++++++++++++++++++++++++++++|
 * | ID length                    | -- byte
 * | color map type               | -- byte
 * | image type                   | -- byte
 * |++++++++++++++++++++++++++++++|
 * | color map specification      | -- 5 bytes(following)
 * | 1. first_entry_index         | -- 2 bytes
 * | 2. color map length          | -- 2 bytes
 * | 3. color map entry_size      | -- bytes
 * |++++++++++++++++++++++++++++++|
 * | image specification          | -- 10 bytes(following)
 * | 1. x_origin                  | -- 2 bytes
 * | 2. y_origin                  | -- 2 bytes
 * | 3. image width               | -- 2 bytes
 * | 4. image length              | -- 2 bytes
 * | 5. pixel depth               | -- byte
 * | 6. image descriptor          | -- byte
 * |++++++++++++++++++++++++++++++|
 */
struct TgaHeader {
  uint8_t id_length = 0;
  uint8_t color_map_type = 0;
  uint8_t image_type = 0;
  uint16_t first_entry_index = 0;
  uint16_t color_map_length = 0;
  uint8_t color_map_entry_size = 0;
  uint16_t x_origin = 0;
  uint16_t y_origin = 0;
  uint16_t width = 0;
  uint16_t height = 0;
  uint8_t pixel_depth = 0; // aka. Bits per pixel(commonly, 8/16/24/32)

  /*
   * 1 bytes(8 bits):
   * |+++++++++++++++++++++++++++++++++++++++++++|
   * | 0 | 0 |       |      |    |     |    |    |
   * | unused| image origin | Alpha channel bits |
   * |+++++++++++++++++++++++++++++++++++++++++++|
   *
   * Alpha channel bits: The number of attribute bits per pixel(bbp)
   * image origin: The order in which pixel data is transferred from the
   *               file to the screen.
   *               (Bit4: left-to-right, aka. ->)
   *               (Bit5: top-to-bottom, aka. down)
   *               BUT 00 --> Left Bottom,
   *               You should think this is the origin of image in the screen.
   */
  uint8_t image_descriptor = 0;
};

struct TgaFooter {
  uint32_t ext_area_offset = 0;
  uint32_t dev_dir_offset = 0;
  char signature[16];
  char dot_ascii = '.';
  char terminator = '\0';
};

// Recover normal alignment requirement
#pragma pack(pop)

static_assert(sizeof(TgaHeader) == 18,
              "The length of TGA header is not required");

static_assert(sizeof(TgaFooter) == 26,
              "The length of TGA footer is not required");

/**
 * TGA format image(*.tga)
 *
 * \see http://tfc.duke.free.fr/coding/tga_specs.pdf
 */
class TgaImage {
 public:
  /**
   * Don't care color-mapped
   */
  enum ImageType {
    NO_IMAGE_DATA = 0,
    GRAYSCALE = 1, // white to black
    RGB = 3,       // Red, Green, Blue channels
    RGBA = 4,      // Attribute(commonly, alpha channel)
  };
  
  /**
   * The origin of image in screen
   */
  enum ImageOriginOrder : uint8_t {
    BOTTOM_LEFT = 0x00,
    BOTTOM_RIGHT = 0x10,
    TOP_LEFT = 0x20,
    TOP_RIGHT = 0x30,
  };
  
  TgaImage();
  TgaImage(uint8_t const *borrow_data, int w, int h, ImageType t= RGB);
  TgaImage(int w, int h, ImageType t = RGB);

  TgaImage(TgaImage const &) = default;
  TgaImage(TgaImage &&) = default;
  TgaImage& operator=(TgaImage const &) = default;
  TgaImage& operator=(TgaImage &&) = default;

  ~TgaImage() noexcept;

  /*--------------------------------------------------*/
  /* File operation                                   */
  /*--------------------------------------------------*/

  bool WriteTo(char const *path,
               bool rle = false,
               ImageOriginOrder order = ImageOriginOrder::BOTTOM_LEFT) noexcept;

  bool ReadFrom(char const *path) noexcept;

  /*--------------------------------------------------*/
  /* Pixel manipulation                               */
  /*--------------------------------------------------*/

  void SetPixel(int x, int y, Color const &color) noexcept;
  Color GetPixel(int x, int y) noexcept;

  /*--------------------------------------------------*/
  /* Getter                                           */
  /*--------------------------------------------------*/

  int bytes_per_pixel() const noexcept { return (int)image_type_; }
  int bits_per_pixel() const noexcept { return bytes_per_pixel() << 3; }
  int width() const noexcept { return width_; }
  int height() const noexcept { return height_; }
  
  void EnableDebug(bool debug) noexcept { enable_debug_ = debug; }

 private:
  void PrintHeader(TgaHeader const &header);

  void Unimplement(char const *msg) noexcept;

  uint16_t width_ = 0;
  uint16_t height_ = 0;

  ImageType image_type_ = NO_IMAGE_DATA;
  
  bool enable_debug_ = false;
  uint8_t const* borrow_image_data_ = nullptr;
  std::vector<uint8_t> image_data_; // TODO Not optimal
};

} // namespace img

#endif // KURO_TGA_IMAGE_H__
