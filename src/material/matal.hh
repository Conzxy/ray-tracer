#ifndef MATERIAL_MATAL_HH__
#define MATERIAL_MATAL_HH__

#include "material.hh"

namespace rt {

class Matal : public Material {
 public:
  explicit Matal(Color const &albedo, double fuzzy)
    : albedo_(albedo)
    , fuzzy_(fuzzy < 1 ? fuzzy : 1)
  {
  }

  virtual bool scatter(const Ray &in_ray, const HitRecord &record, ScatterRecord &srec) const override;
 private:
  Color albedo_;
  double fuzzy_;
};

} // namespace rt

#endif
