#ifndef SOLID_TEXTURE_HH__
#define SOLID_TEXTURE_HH__

#include "texture.hh"

namespace rt {

class SolidTexture : public Texture {
 public:
  SolidTexture() = default;
  SolidTexture(rt::Color const &color)
    : color_(color)
  {
  }

  rt::Color value(double u, double v, Point3F const &p) const override
  {
    return color_;
  }
 private:
  rt::Color color_;
};

} // namespace rt

#endif
