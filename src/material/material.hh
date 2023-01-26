#ifndef MATERIAL_HH__
#define MATERIAL_HH__

#include "../rt/color.hh"
#include "../gm/point.hh"

#include "type.hh"

namespace rt {

class Ray;
class HitRecord;

class Material
{
 public:
  virtual bool scatter(Ray const &in_ray, HitRecord const &record, Color *attenuation, Ray *out_ray) const = 0;
  virtual Color emitted(double u, double v, Point3F const &p) const
  {
    return { 0, 0, 0 };
  }
};

} // namespace rt

#endif
