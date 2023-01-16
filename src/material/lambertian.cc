#include "lambertian.hh"

#include "../rt/color.hh"
#include "../rt/hit_record.hh"
#include "../rt/ray.hh"

using namespace rt;
using namespace gm;

bool Lambertian::scatter(Ray const &in_ray, HitRecord const &record,
                         Color *attenuation, Ray *out_ray) const
{
  if (albedo_ == Vec3F(0, 0, 0)) {
    return false;
  }
  auto direction = record.normal + random_on_unit_sphere_surface();
  if (direction.is_near_zero()) {
    // FIXME Retry?
    direction = record.normal;
  }
  *attenuation = albedo_;
  *out_ray = Ray(record.p, direction);
  return true;
}
