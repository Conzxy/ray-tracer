#include "dielectric.hh"

#include "../rt/hit_record.hh"
#include "../rt/scatter_record.hh"
#include "../gm/util.hh"
#include "util.hh"

using namespace gm;
using namespace rt;

inline static double schelink_reflectance(double cosine, double refract_ratio)
{
  auto r0 = (1- refract_ratio) / (1 + refract_ratio);
  r0 *= r0;
  return r0 + (1-r0)*pow((1-cosine), 5);  
}

bool Dielectric::scatter(const Ray &in_ray, const HitRecord &record, Color *attenuation, Ray *out_ray, ScatterRecord *s_rec) const
{
  *attenuation = rt::Color(1.0, 1.0, 1.0);  

  // 针对表面法向量n，取外侧（法向量相对侧)/内侧(法向量这一侧)的折射率作为构造参数表示折射率比值
  // 如果交点角度来看，法向量朝内，说明折射率是内侧/外侧，需要取倒数
  // 而法向量朝外，内外侧反转，说明折射率就是原来的外侧/原来的内侧，不需要取倒数
  double refract_ratio = record.front_face ? (1.0/rr_) : rr_;

  auto unit_direciton = in_ray.direction().normalize();
  double cos_theta = fmin(dot(-unit_direciton, record.normal), 1.0);

  bool cannot_refract = refract_ratio * gm::sin_from_cos(cos_theta) > 1.0;

  Vec3F out_ray_direction;
  if (cannot_refract || schelink_reflectance(cos_theta, refract_ratio) > util::random_double())
    out_ray_direction = reflect(unit_direciton, record.normal);
  else
    out_ray_direction = refract(unit_direciton, record.normal, refract_ratio);

  *out_ray = Ray(record.p, out_ray_direction);
  s_rec->pdf = dot(record.normal, unit_direciton) / pi;
  return true;
}
