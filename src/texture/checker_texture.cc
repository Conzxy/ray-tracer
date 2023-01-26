#include "checker_texture.hh"

#include "solid_texture.hh"

using namespace rt;

CheckerTexture::CheckerTexture(TextureSPtr even, TextureSPtr odd)
  : even_(std::move(even))
  , odd_(std::move(odd))
{
}

CheckerTexture::CheckerTexture(Color const &c1, Color const &c2)
  : CheckerTexture(std::make_shared<SolidTexture>(c1), std::make_shared<SolidTexture>(c2))
{
}

Color CheckerTexture::value(double u, double v, Point3F const &p) const
{
  auto sines = sin(p.x*10) * sin(p.y*10) * sin(p.z*10);
  if (sines < 0)
    return odd_->value(u, v, p);
  else
    return even_->value(u, v, p);
}
