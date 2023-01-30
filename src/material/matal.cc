#include "matal.hh"

#include <cassert>

#include "../gm/util.hh"
#include "../rt/hit_record.hh"
#include "../rt/scatter_record.hh"
#include "util.hh"

using namespace rt;
using namespace gm;

bool Matal::scatter(const Ray &in_ray, const HitRecord &record,
                    ScatterRecord &srec) const
{
  auto reflect_light = reflect(in_ray.direction(), record.normal);
  assert(dot(reflect_light, record.normal) > 0);
  // 如果dot() <
  // 0，表示出射光射向了表面，这是由于模糊参数(fuzzy_)过大导致的，故舍弃该光线（过于模糊不遵循镜面反射规则）
  srec.is_specular = true;
  srec.attenuation = albedo_;
  srec.specular_ray =
      Ray(record.p, reflect_light + fuzzy_ * gm::random_in_unit_sphere());
  srec.pdf = nullptr;
  return dot(srec.specular_ray.direction(), record.normal);
}
