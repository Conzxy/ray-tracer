#ifndef SHAPE_SPHERE_HH__
#define SHAPE_SPHERE_HH__

#include "shape.hh"

namespace rt {

class Material;

class Sphere : public Shape
{
 public:
  Sphere(Point3F const &center, double radius, Material *material)
    : center_(center)
    , radius_(radius)
    , material_(material)
  {
  }
    
  bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const override;

  Vec3F normal(Point3F const &p) const noexcept;
 private:
  Point3F center_;
  double radius_;

  Material *material_;
};

} // namespace rt

#endif
