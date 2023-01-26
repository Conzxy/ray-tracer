#include "diffuse_light.hh"

#include "../rt/hit_record.hh"
#include "../texture/solid_texture.hh"

using namespace rt;

DiffuseLight::DiffuseLight(rt::Color const &c)
  : DiffuseLight(std::make_shared<SolidTexture>(c))
{
}

Color DiffuseLight::emitted(HitRecord const &rec, double u, double v, Point3F const &p) const
{
  if (rec.front_face)
    return emit_->value(u, v, p);
  else
    return {0, 0, 0};   
}