#include "lambertian.hh"

#include "../texture/solid_texture.hh"
#include "../rt/color.hh"
#include "../rt/hit_record.hh"
#include "../rt/scatter_record.hh"
#include "../rt/ray.hh"
#include "../gm/util.hh"

using namespace rt;
using namespace gm;

Lambertian::Lambertian(Color const &albedo)
  : albedo_(std::make_shared<SolidTexture>(albedo))
{
}

bool Lambertian::scatter(Ray const &in_ray, HitRecord const &record,
                         Color *attenuation, Ray *out_ray, ScatterRecord *s_rec) const
{
  auto albedo_value = albedo_->value(record.u, record.v, record.p);
  if (albedo_value == Vec3F(0, 0, 0)) {
    return false;
  }
  auto direction = record.normal + random_on_unit_sphere_surface();
  if (direction.is_near_zero()) {
    // FIXME Retry?
    direction = record.normal;
  }
  *attenuation = albedo_value;
  *out_ray = Ray(record.p, direction);
  s_rec->pdf = dot(in_ray.direction().normalize(), record.normal) / pi;
  return true;
}
