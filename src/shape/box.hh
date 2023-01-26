#ifndef RT_SHAPE_BOX_HH__
#define RT_SHAPE_BOX_HH__

#include "shape.hh"
#include "../shape/shape_list.hh"

#include "../material/type.hh"
#include "../gm/point.hh"

namespace rt {

class Box : public Shape {
 public:
  Box(gm::Point3F const &bottom, gm::Point3F const &top, MaterialSPtr const &material);

  bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const override;
  bool get_bounding_box(Aabb &bbox) const override;
 private:
  gm::Point3F top_;
  gm::Point3F bottom_;
  ShapeList faces_;
};

}

#endif
