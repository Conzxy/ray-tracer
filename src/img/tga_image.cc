#include "tga_image.hh"

#include <algorithm>
#include <cassert>
#include <cstring>

#include "../util/assert.hh"
#include "../util/file.hh"
#include "../util/endian.h"

using namespace img;
using namespace util;

#define NEW_FORMAT_SIGNATURE "TRUEVISION-XFILE"

static char const *
ImageOriginOrder2Str(TgaImage::ImageOriginOrder order) noexcept
{
  switch (order) {
    case TgaImage::BOTTOM_LEFT:
      return "Bottom left";
    case TgaImage::BOTTOM_RIGHT:
      return "Bottom right";
    case TgaImage::TOP_LEFT:
      return "Top left";
    case TgaImage::TOP_RIGHT:
      return "Top right";
  }

  return "Unknown";
}

static uint8_t MakeTgaRlePacket(uint8_t is_rle, uint8_t pixel_num) noexcept
{
  assert(is_rle <= 1);
  assert(pixel_num <= 128 && pixel_num > 0);
  return (is_rle << 7) | ((pixel_num - 1) & 0x7f);
}

TgaImage::TgaImage() = default;

TgaImage::TgaImage(int w, int h, ImageType t)
  : width_(w)
  , height_(h)
  , image_type_(t)
  , image_data_(bytes_per_pixel() * w * h, 0)
{
}

TgaImage::TgaImage(uint8_t const *borrow_data, int w, int h, ImageType t)
  : width_(w)
  , height_(h)
  , image_type_(t)
  , borrow_image_data_(borrow_data)
{
}

TgaImage::~TgaImage() noexcept {}

bool TgaImage::WriteTo(char const *path, bool rle,
                       ImageOriginOrder order) noexcept
{
  if (enable_debug_) {
    printf("===== WriteTo debug(start) =====\n");
  }

  File out;
  if (!out.Open(path, File::TRUNC)) {
    fprintf(stderr, "Failed to write tga file: %s\n", path);
    return false;
  }

  // NOTICE Must be little endian
  TgaHeader header;
  header.pixel_depth = bits_per_pixel();
  header.height = htole16(height_);
  header.width = htole16(width_);
  switch (image_type_) {
    case NO_IMAGE_DATA:
      header.image_type = 0;
      break;
    case GRAYSCALE:
      header.image_type = rle ? 11 : 3;
      break;
    case RGB:
    case RGBA:
      header.image_type = rle ? 10 : 2;
      break;
  }
  header.image_descriptor |= (uint8_t)order;
  
  out.Write(&header, sizeof(header));
  if (enable_debug_)
    PrintHeader(header);
  /*
   * After header:
   * Image/Color Map data:
   * |++++++++++++++++|
   * | image ID       | variable
   * | color map data | variable
   * | image data     | variable
   * |++++++++++++++++|
   */

  /* Don't care color map data and image ID now */
  uint8_t const *image_raw_data =
      borrow_image_data_ ? borrow_image_data_ : image_data_.data();
  size_t pixel_total = width_ * height_;
  size_t pixel_size = pixel_total * bytes_per_pixel();
  uint8_t const *image_raw_data_end = image_raw_data + pixel_size;

  if (rle) {
    std::vector<uint8_t> image_buffer;
    image_buffer.reserve(pixel_size);

    size_t pixel_index = 0;
    int bpp = bytes_per_pixel();
    int pixel_bytes = 0;
    int pixel_number = 0;

    size_t raw_packet_num = 0;
    size_t rle_packet_num = 0;

    for (; pixel_index < pixel_size;) {
      // 存在相邻元素
      if (pixel_index < pixel_size - bpp) {
        auto first_pixel = image_raw_data + pixel_index;
        auto second_pixel = first_pixel + bpp;
        
        // 如果相邻像素值相同，考虑Run-Length Packet
        // 找到下一个不同于这两个像素的像素位置
        //
        // WARNING 不要用STL <algorithm>，因为不可以指定步长
        if (memcmp(first_pixel, second_pixel, bpp) == 0) {
          uint8_t const *next_diff_pixel = nullptr;
          second_pixel += bpp;
          for (; second_pixel < image_raw_data_end; second_pixel += bpp) {
            if (memcmp(second_pixel, first_pixel, bpp) != 0) {
              next_diff_pixel = second_pixel;
              break;
            }
          }
          
          if (rle_enable_debug_) {
            printf("[RLE packet]\n");
            printf("pixel number = %d\n", pixel_number);
          }

          if (!next_diff_pixel) next_diff_pixel = image_raw_data_end;
          pixel_bytes = next_diff_pixel - first_pixel;
          pixel_number = pixel_bytes / bpp;
          
          for (; pixel_number >= 128; pixel_number -= 128) {
            image_buffer.push_back(MakeTgaRlePacket(1, 128));

            // 只需要第一个像素即可，因为均相同
            image_buffer.insert(image_buffer.end(), first_pixel,
                                first_pixel + bpp);
            rle_packet_num++;
          }

          if (pixel_number > 0) {
            image_buffer.push_back(MakeTgaRlePacket(1, pixel_number));
            image_buffer.insert(image_buffer.end(), first_pixel,
                                first_pixel + bpp);
            rle_packet_num++;
          }
        } else {
          // 不相同考虑Raw Packet
          auto saved_first_pixel = first_pixel;
          uint8_t const *next_adj_same_pixel = nullptr;
          
          first_pixel += bpp;
          second_pixel += bpp;
          for (; second_pixel < image_raw_data_end;) {
            if (memcmp(second_pixel, first_pixel, bpp) == 0) {
              next_adj_same_pixel = first_pixel;
              break;
            }
            first_pixel += bpp;
            second_pixel += bpp;
          }
        
          if (!next_adj_same_pixel) next_adj_same_pixel = image_raw_data_end;
          pixel_bytes = next_adj_same_pixel - saved_first_pixel;
          pixel_number = pixel_bytes / bpp;

          if (rle_enable_debug_) {
            printf("[Raw packet]\n");
            printf("pixel_number = %d\n", pixel_number);
          }
          
          assert(pixel_number > 0);
          for (; pixel_number >= 128; pixel_number -= 128) {
            auto end = saved_first_pixel + 128 * bpp;
            image_buffer.push_back(MakeTgaRlePacket(0, 128));
            image_buffer.insert(image_buffer.end(), saved_first_pixel, end);
            saved_first_pixel = end;
            raw_packet_num++;
          }

          if (pixel_number > 0) {
            image_buffer.push_back(MakeTgaRlePacket(0, pixel_number));
            image_buffer.insert(image_buffer.end(), saved_first_pixel,
                                next_adj_same_pixel);
            raw_packet_num++;
          }
        }
        pixel_index += pixel_bytes;
      } else if (pixel_index == pixel_size - bpp) {
        image_buffer.push_back(MakeTgaRlePacket(0, 1));
        auto pixel = image_raw_data + pixel_index;
        image_buffer.insert(image_buffer.end(), pixel, pixel + bpp);
        pixel_index += bpp;
        raw_packet_num++;
      }
    }

    if (enable_debug_) {
      printf("raw packet num = %zu\n", raw_packet_num);
      printf("rle packet num = %zu\n", rle_packet_num);
    }
    out.Write(image_buffer.data(), image_buffer.size());
  } else {
    out.Write(image_raw_data, pixel_size);
  }
  
  /*
   * Developer area
   * and Extension area ignored also
   */

  /**
   * Append footer to indicates this is new version tga format
   */
  TgaFooter footer;
  footer.dev_dir_offset = htole32(footer.dev_dir_offset);
  footer.ext_area_offset = htole32(footer.ext_area_offset);
  memcpy(footer.signature, NEW_FORMAT_SIGNATURE, sizeof footer.signature);

  out.Write(&footer, sizeof footer);
  
  if (enable_debug_) {
    printf("===== WriteTo debug(end) =====\n");
  }
  return true;
}

bool TgaImage::ReadFrom(char const *path) noexcept
{
  if (enable_debug_) {
    printf("ReadFrom debug\n");
  }

  File in;
  if (!in.Open(path, File::READ)) {
    fprintf(stderr, "Failed to read tga file: %s\n", path);
    return false;
  }

  TgaFooter footer;

  bool is_new_format = true;
  (void)is_new_format;

  if (!in.SeekEnd(sizeof(TgaFooter))) {
    if (errno == EINVAL) {
      is_new_format = false;
    }
  }

  if (in.Read(&footer, sizeof footer) == File::INVALID_RETURN) {
    fprintf(stderr, "Failed to read footer of TGA image: %s\n", path);
    return false;
  }

  if (memcpy(footer.signature, NEW_FORMAT_SIGNATURE, sizeof footer.signature) !=
      0)
  {
    is_new_format = false;
  } else {
    footer.ext_area_offset = le32toh(footer.ext_area_offset);
    footer.dev_dir_offset = le32toh(footer.dev_dir_offset);
  }

  if (!in.SeekBegin(0)) {
    fprintf(stderr, "Failed to seek TGA image: %s\n", path);
    return false;
  }

  TgaHeader header;
  in.Read(&header, sizeof header);

  header.first_entry_index = le16toh(header.first_entry_index);
  header.color_map_entry_size = le16toh(header.color_map_entry_size);

  /*
   * The purpose of x/y_origin of image see
   * https://stackoverflow.com/questions/25113149/in-the-tga-file-format-what-is-the-purpose-of-the-x-origin-and-y-origin
   */
  header.x_origin = le16toh(header.x_origin);
  header.y_origin = le16toh(header.y_origin);
  header.width = le16toh(header.width);
  header.height = le16toh(header.height);

  width_ = header.width;
  height_ = header.height;

  auto image_origin_order =
      ImageOriginOrder((header.image_descriptor & 0x30) >> 4);
  auto alpha_bits = header.image_descriptor & 0x0f;

  bool rle = false;
  switch (header.image_type) {
    case 0:
      image_type_ = NO_IMAGE_DATA;
      break;
    case 1:
    case 9:
      Unimplement("Color-mapped image");
      if (header.image_type == 9) rle = true;
      break;
    case 2:
    case 10:
      image_type_ = alpha_bits > 0 ? RGBA : RGB;
      if (header.image_type == 10) rle = true;
      break;
    case 3:
    case 11:
      image_type_ = GRAYSCALE;
      if (header.image_type == 11) rle = true;
      break;
    default:
      fprintf(stderr, "Unknown image type\n");
      return false;
  }

  if (enable_debug_) {
    PrintHeader(header);
    printf("image origin order = %s\n",
           ImageOriginOrder2Str(image_origin_order));
    printf("alpha channel bits = %d\n", (int)alpha_bits);
    printf("rle = %d\n", rle ? 1 : 0);
    printf("bytes per pixel = %d\n", bytes_per_pixel());
    printf("new format: %d\n", is_new_format ? 1 : 0);
    printf("Extension area offset = %d\n", footer.ext_area_offset);
    printf("Developer directory offset = %d\n", footer.dev_dir_offset);
  }

  size_t color_map_size =
      (header.color_map_type == 0)
          ? 0
          : (header.color_map_length * header.color_map_entry_size);
  in.SeekCurrent(header.id_length + color_map_size);
  
#if 0
  auto file_size = in.GetFileSize();
  auto cur_pos = sizeof header + header.id_length + color_map_size;
  size_t image_data_size = 0;
  if (is_new_format) {
    image_data_size = (footer.ext_area_offset == 0 ? (file_size - 26)
                                                   : footer.ext_area_offset) -
                      cur_pos;
  } else {
    image_data_size = file_size - cur_pos;
  }
#endif

  /*
   * RLE(Run-length encoding
   * Run-length packets & Raw packets
   *
   * (1st field: Repetition Count field)The first 1 byte of each packet.
   * (2ed field: Pixel Value field)For Run-time packet, single pixel value, for
   * raw packet, variable number of pixel value.
   *
   * If the bit7 of 1st field is 1 ==> packet is Run-length packet, otherwise,
   * raw packet. For Run-length packet, lower 7 bits indicates the number of
   * pixel data with same data. For Raw packet, lower 7 bits indicates the pixel
   * data with different color. NOTICE, the number is less than the actual
   * number, i.e. 1 encoded to 0, 128 encoded to 127.
   */
  const auto bpp = bytes_per_pixel();
  image_data_.resize(header.width * header.height * bpp, 0);

  size_t rle_packet_num = 0;
  size_t raw_packet_num = 0;

  if (rle) {
    uint8_t packet_header;
    size_t cached_pixel_size = 0;
    std::vector<uint8_t> pixel_buffer(128 * bpp);

    while (cached_pixel_size < image_data_.size()) {
      in.Read(&packet_header, sizeof packet_header);
      auto packet_type = (packet_header & 0x80);
      auto pixel_number = (packet_header & 0x7f) + 1;
      auto pixel_size = pixel_number * bpp;

      if (rle_enable_debug_) {
        printf("pixel_number = %d\n", pixel_number);
      }
      // Raw packet
      if (packet_type == 0) {
        if (rle_enable_debug_) printf("Raw packet\n");
        in.Read(&pixel_buffer[0], pixel_size);
        memcpy(&image_data_[cached_pixel_size], pixel_buffer.data(),
               pixel_size);
        raw_packet_num++;
      } else {
        // Run-length packet
        if (rle_enable_debug_) printf("RLE packet\n");
        in.Read(&pixel_buffer[0], bpp);
        for (int i = 0; i < pixel_size; i += bpp) {
          memcpy(&image_data_[cached_pixel_size + i], pixel_buffer.data(),
                 bpp);
        }
        rle_packet_num++;
      }

      cached_pixel_size += pixel_size;
    }
  } else {
    in.Read(&image_data_[0], image_data_.size());
  }
  
  if (rle_enable_debug_) {
    printf("rle packet number = %zu\n", rle_packet_num);
    printf("raw packet number = %zu\n", raw_packet_num);
  }
  return true;
}

void TgaImage::SetPixel(int x, int y, Color const &color) noexcept
{
  ASSERT(x >= 0 && x < width_ && y >= 0 && y < height_);

  const auto bpp = bytes_per_pixel();
  memcpy(&image_data_[(x + y * width_) * bpp], &color, bpp);
}

Color TgaImage::GetPixel(int x, int y) noexcept
{
  ASSERT(x >= 0 && x < width_ && y >= 0 && y < height_);

  Color ret;
  const auto bpp = bytes_per_pixel();
  memcpy(&ret, &image_data_[(x + y * width_) * bpp], bpp);

  return ret;
}

void TgaImage::Unimplement(char const *msg) noexcept
{
  fprintf(stderr, "%s is unimplement\n", msg);
}

void TgaImage::PrintHeader(TgaHeader const &header)
{
  printf("id_length = %d\n", (int)header.id_length);
  printf("color_map_type = %d\n", (int)header.color_map_type);
  printf("image_type = %d\n", (int)header.image_type);
  printf("first_entry_index = %d\n", (int)header.first_entry_index);
  printf("color_map_length = %d\n", (int)header.color_map_length);
  printf("color_map_entry_size = %d\n", (int)header.color_map_entry_size);
  printf("x_origin = %d\n", (int)header.x_origin);
  printf("y_origin = %d\n", (int)header.y_origin);
  printf("width = %d\n", (int)header.width);
  printf("height = %d\n", (int)header.height);
  printf("pixel_depth = %d\n", (int)header.pixel_depth);
  printf("image_descriptor = %x\n", (int)header.image_descriptor);
}
