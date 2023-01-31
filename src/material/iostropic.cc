#include "iostropic.hh"
#include "../texture/solid_texture.hh"
#include "../rt/scatter_record.hh"
#include "../rt/hit_record.hh"

using namespace rt;
using namespace std;
using namespace gm;

Iostropic::Iostropic(Color c)
  : Iostropic(make_shared<SolidTexture>(c))
{
}

bool Iostropic::scatter(Ray const &ray, HitRecord const &record, ScatterRecord &srec) const
{
  srec.is_specular = true;
  srec.specular_ray = Ray(record.p, random_in_unit_sphere());
  srec.attenuation = albedo_->value(record.u, record.v, record.p);
  return true;
}
