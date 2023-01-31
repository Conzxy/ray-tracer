#ifndef RT_MATERIAL_OPSTROPIC_HH__
#define RT_MATERIAL_OPSTROPIC_HH__

#include "../texture/texture.hh"
#include "material.hh"

namespace rt {

class Iostropic : public Material {
 public:
  explicit Iostropic(TextureSPtr albedo)
    : albedo_(std::move(albedo))
  {
  }

  explicit Iostropic(Color c);

  virtual bool scatter(Ray const &ray, HitRecord const &record,
                       ScatterRecord &srec) const override;

 private:
  TextureSPtr albedo_;
};

} // namespace rt

#endif
