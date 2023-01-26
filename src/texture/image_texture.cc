#include "image_texture.hh"

#include <stdio.h>

#include "../gm/util.hh"
#include "../img/rt_image.hh"
#include "../util/string_util.h"

using namespace rt;
using namespace util;
using namespace gm;

ImageTexture::ImageTexture(char const *path)
{
  data_ = stbi_load(path, &width_, &height_, &bytes_per_pixel_, 3);
  if (!data_) {
    throw std::runtime_error(StrCat("Failed to load image file: ", path));
  }
}

ImageTexture::~ImageTexture() noexcept
{
  if (data_) stbi_image_free(data_);
}

Color ImageTexture::value(double u, double v, Point3F const &p) const
{
  assert(data_);

  u = clamp(u, 0, 1);
  v = 1. - clamp(v, 0, 1);

  int x = u * width_;
  x = x >= width_ ? (width_ - 1) : x;
  int y = v * height_;
  y = y >= height_ ? (height_ - 1) : y;

  int index = y * bytes_per_pixel_ * width_ + x * bytes_per_pixel_;
  auto color_scale = 1. / 255.;

  return {
      data_[index] * color_scale,
      data_[index + 1] * color_scale,
      data_[index + 2] * color_scale,
  };
}

namespace rt {

std::ostream &operator<<(std::ostream &os, rt::ImageTexture const &tex)
{
  return os << "===== Image Texture Info(start) =====\n"
            << "width = " << tex.width_ << '\n'
            << "height = " << tex.height_ << '\n'
            << "bytes_per_pixel = " << tex.bytes_per_pixel_ << '\n'
            << "===== Image Texture Info(end) =====";
}

} // namespace rt
