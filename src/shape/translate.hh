#ifndef RT_SHAPE_TRANSLATE_HH__
#define RT_SHAPE_TRANSLATE_HH__

#include "shape.hh"

#include "../gm/vec.hh"

namespace rt {

class Translate : public Shape {
 public:
  Translate(ShapeSPtr shape, gm::Vec3F const &offset)
    : shape_(std::move(shape))
    , offset_(offset)
  {
  }

  bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const override;
  bool get_bounding_box(Aabb &bbox) const override;
 private:
  ShapeSPtr shape_;
  gm::Vec3F offset_;
};

}

#endif
