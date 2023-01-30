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

  virtual double pdf_value(Point3F const &origin,
                           Vec3F const &direction) const override
  {
    return shape_->pdf_value(origin, direction);
  }
  virtual Vec3F random_direction(Point3F const &origin) const override
  {
    return shape_->random_direction(origin);
  }

  virtual bool hit(Ray const &ray, double tmin, double tmax,
                   HitRecord &rec) const override;
  virtual bool get_bounding_box(Aabb &bbox) const override;

 private:
  ShapeSPtr shape_;
};

} // namespace rt

#endif