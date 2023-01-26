#ifndef MATERIAL_DIFFUSE_LIGHT_HH__
#define MATERIAL_DIFFUSE_LIGHT_HH__

#include "material.hh"

#include "../texture/texture.hh"

namespace rt {

/**
 * Emissive material
 * 可以贴在物体上使它成为发光体(拥有位置和大小)
 * 比固定为一个点的光源或是只有一个方向的光源(平行光源)要好
 * (同样的思路要实现这两种光源也很简单)
 */
class DiffuseLight : public Material {
 public:
  explicit DiffuseLight(TextureSPtr emit)
    : emit_(emit)
  {
  }

  explicit DiffuseLight(Color const &emit);

  bool scatter(Ray const &in_ray, HitRecord const &record, Color *attenuation,
               Ray *out_ray, ScatterRecord *) const override
  {
    return false;
  }
  
  Color emitted(HitRecord const &rec, double u, double v, Point3F const &p) const override;

  virtual bool is_emissive() const override { return true; }
 private:
  TextureSPtr emit_;
};

} // namespace rt
#endif
