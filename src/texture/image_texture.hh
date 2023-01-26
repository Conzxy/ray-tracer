#ifndef TEXTURE_IMAGE_TEXTURE_HH__
#define TEXTURE_IMAGE_TEXTURE_HH__

#include <iosfwd>

#include "texture.hh"

namespace rt {

// class ImageTexture;

// std::ostream &operator<<(std::ostream &os, ImageTexture const &tex);

class ImageTexture : public Texture {
 public:
  explicit ImageTexture(char const *path);
  ~ImageTexture() noexcept;

  Color value(double u, double v, Point3F const &p) const override;

  friend std::ostream &operator<<(std::ostream &os, ImageTexture const &tex);
 private:
  int width_ = 0;
  int height_ = 0;
  int bytes_per_pixel_ = -1;
  unsigned char *data_ = nullptr;
};

} // namespace rt

#endif
