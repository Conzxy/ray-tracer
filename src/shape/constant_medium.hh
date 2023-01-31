#ifndef RT_CONSTANT_MEDIUM_HH__
#define RT_CONSTANT_MEDIUM_HH__

#include "shape.hh"
#include "../texture/texture.hh"
#include "../material/material.hh"

namespace rt {

class ConstantMedium : public Shape {
 public:
  ConstantMedium(ShapeSPtr &&boundary, double density, TextureSPtr albedo);
  ConstantMedium(ShapeSPtr &&boundary, double density, Color albedo);

  virtual bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const override;
  virtual bool get_bounding_box(Aabb &output_box) const override;
 private:
  ShapeSPtr boundary_;
  double density_;
  MaterialSPtr phase_function_;
};

} // namespace rt

#endif
