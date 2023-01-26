#include "diffuse_light.hh"

#include "../texture/solid_texture.hh"

using namespace rt;

DiffuseLight::DiffuseLight(rt::Color const &c)
  : DiffuseLight(std::make_shared<SolidTexture>(c))
{
}
