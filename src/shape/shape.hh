#ifndef SHAPE_HH__
#define SHAPE_HH__

#include <memory>

#include "../accelerate/aabb.hh"
#include "../rt/ray.hh"

namespace rt {

struct HitRecord;

class Shape
{
 public:
  virtual bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const = 0;
  virtual bool get_bounding_box(Aabb &output_box) const = 0;
  Aabb get_bounding_box() const;
};

using ShapePtr = std::shared_ptr<Shape>;
using ShapeSPtr = std::shared_ptr<Shape>;

} // namespace rt

#endif
