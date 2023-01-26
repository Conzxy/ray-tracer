#ifndef SHAPE_SPHERE_HH__
#define SHAPE_SPHERE_HH__

#include "shape.hh"

#include "../material/type.hh"

namespace rt {

class Material;

class Sphere : public Shape
{
 public:
  Sphere(Point3F const &center, double radius, MaterialSPtr material)
    : center_(center)
    , radius_(radius)
    , material_(std::move(material))
  {
  }
    
  bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const override;
  bool get_bounding_box(Aabb &output_box) const override;

  Vec3F normal(Point3F const &p) const noexcept;

  static void get_uv(Point3F const &p, double &u, double &v);
 private:
  Point3F center_;
  double radius_;

  MaterialSPtr material_;
};

} // namespace rt

#endif
