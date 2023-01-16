#ifndef SHAPE_HH__
#define SHAPE_HH__

#include <memory>

#include "../rt/ray.hh"

namespace rt {

class HitRecord;

class Shape
{
 public:
  virtual bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const = 0;
};

using ShapePtr = std::shared_ptr<Shape>;

} // namespace rt

#endif
