#ifndef RT_SCATTER_RECORD_HH__
#define RT_SCATTER_RECORD_HH__

#include "../sample/pdf.hh"
#include "../rt/ray.hh"
#include "../rt/color.hh"

namespace rt {

struct ScatterRecord {
  Ray specular_ray;
  bool is_specular = true;
  Color attenuation;
  PdfSPtr pdf = nullptr;
};

} // namespace rt

#endif
