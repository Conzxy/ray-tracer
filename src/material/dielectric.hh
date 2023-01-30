#ifndef MATERIAL_DIELECTRIC_HH__
#define MATERIAL_DIELECTRIC_HH__

#include "material.hh"

namespace rt {

class Dielectric : public Material {
 public:
  explicit Dielectric(double ratio_of_refraction)
    : rr_(ratio_of_refraction)
  {
  }

  bool scatter(const Ray &in_ray, const HitRecord &record, ScatterRecord &srec) const override;

 private:
  double rr_;
};

} // namespace rt

#endif