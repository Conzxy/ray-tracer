#include "lambertian.hh"

#include "../gm/util.hh"
#include "../rt/color.hh"
#include "../rt/hit_record.hh"
#include "../rt/ray.hh"
#include "../rt/scatter_record.hh"
#include "../sample/cosine_pdf.hh"
#include "../texture/solid_texture.hh"

using namespace rt;
using namespace gm;

Lambertian::Lambertian(Color const &albedo)
  : albedo_(std::make_shared<SolidTexture>(albedo))
{
}

bool Lambertian::scatter(Ray const &in_ray, HitRecord const &record,
                         ScatterRecord &srec) const
{
  auto albedo_value = albedo_->value(record.u, record.v, record.p);
  srec.is_specular = false;
  srec.attenuation = albedo_value;
  srec.pdf = std::make_shared<CosinePdf>(record.normal);
  return true;
}
