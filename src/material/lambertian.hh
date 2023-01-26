#ifndef MATERIAL_LAMBERTIAN_HH__
#define MATERIAL_LAMBERTIAN_HH__

#include "material.hh"

#include "../rt/color.hh"
#include "../texture/texture.hh"

namespace rt {

class Lambertian : public Material {
 public:
  explicit Lambertian(Color const &albedo);
  explicit Lambertian(TextureSPtr const &texture)
    : albedo_(texture)
  {
  }

  bool scatter(Ray const &in_ray, HitRecord const &record, Color *attenuation,
               Ray *out_ray) const override;

 private:
  TextureSPtr albedo_;
};

} // namespace rt

#endif
