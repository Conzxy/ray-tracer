#ifndef MATERIAL_HH__
#define MATERIAL_HH__

#include "../rt/color.hh"

namespace rt {

class Ray;
class HitRecord;

class Material
{
 public:
  virtual bool scatter(Ray const &in_ray, HitRecord const &record, Color *attenuation, Ray *out_ray) const = 0;
};

} // namespace rt

#endif
