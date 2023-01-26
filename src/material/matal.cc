#include "matal.hh"

#include <cassert>

#include "util.hh"
#include "../rt/hit_record.hh"
#include "../rt/scatter_record.hh"
#include "../gm/util.hh"

using namespace rt;
using namespace gm;

bool Matal::scatter(const Ray &in_ray, const HitRecord &record,
                    Color *attenuation, Ray *out_ray, ScatterRecord *s_rec) const
{
  auto reflect_light = reflect(in_ray.direction(), record.normal);
  *out_ray = Ray(record.p, reflect_light + fuzzy_ * gm::random_in_unit_sphere());
  *attenuation = albedo_;
  assert(dot(reflect_light, record.normal) > 0); 
  // 如果dot() < 0，表示出射光射向了表面，这是由于模糊参数(fuzzy_)过大导致的，故舍弃该光线（过于模糊不遵循镜面反射规则）
  s_rec->pdf = dot(in_ray.direction().normalize(), record.normal) / pi;
  return dot(out_ray->direction(), record.normal);
}
