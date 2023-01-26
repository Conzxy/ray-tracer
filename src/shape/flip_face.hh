#ifndef RT_SHAPE_FLIP_FACE_HH__
#define RT_SHAPE_FLIP_FACE_HH__

#include "shape.hh"

namespace rt {

class FlipFace : public Shape {
 public:
  explicit FlipFace(ShapeSPtr &&shape)
   : shape_(std::move(shape))
  {
  }

  virtual bool hit(Ray const &ray, double tmin, double tmax, HitRecord &rec) const;
  virtual bool get_bounding_box(Aabb &bbox) const;
 private:
  ShapeSPtr shape_;
};

}

#endif