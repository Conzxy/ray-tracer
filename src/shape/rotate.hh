#ifndef RT_SHAPE_ROTATE_HH__
#define RT_SHAPE_ROTATE_HH__

#include "shape.hh"
#include "../gm/matrix.hh"
#include "../accelerate/aabb.hh"

namespace rt {

struct Degree {
  double x = 0;
  double y = 0;
  double z = 0;
};

class Rotate : public Shape {
 public:
  Rotate(ShapeSPtr shape, Degree const &degree);

  virtual bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const override;
  virtual bool get_bounding_box(Aabb &output_box) const override;
 private:
  ShapeSPtr shape_;
  gm::Matrix3x3F reverse_rotate_mat_;
  gm::Matrix3x3F rotate_mat_;
  Aabb cache_bbox_;
  bool has_bbox_;
};

}

#endif