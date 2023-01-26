#ifndef SHAPE_LIST_HH__
#define SHAPE_LIST_HH__

#include "shape.hh"

#include <vector>

namespace rt {

class ShapeList : public Shape {
 public:
  ShapeList() = default;

  void add(ShapePtr const &shape) { shapes_.push_back(shape); }

  bool hit(Ray const &ray, double tmin, double tmax,
           HitRecord &record) const override;

  bool get_bounding_box(Aabb &output_box) const override;

  std::vector<ShapePtr> const &shape() const noexcept { return shapes_; }

 private:
  std::vector<ShapePtr> shapes_;
};

} // namespace rt

#endif
