#ifndef MATERIAL_LAMBERTIAN_HH__
#define MATERIAL_LAMBERTIAN_HH__

#include "material.hh"

#include "../rt/color.hh"

namespace rt {

class Lambertian : public Material {
 public:
  explicit Lambertian(Color const &albedo)
    : albedo_(albedo)
  {
  }

  bool scatter(Ray const &in_ray, HitRecord const &record, Color *attenuation,
               Ray *out_ray) const override;

 private:
  Color albedo_;
};

} // namespace rt

#endif
