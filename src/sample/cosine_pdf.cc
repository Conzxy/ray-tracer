#include "cosine_pdf.hh"

#include <cassert>

#include "../gm/util.hh"
#include "../sample/sample.hh"

using namespace rt;
using namespace gm;

CosinePdf::CosinePdf(Vec3F const &normal)
  : onb_(normal)
{
}

double CosinePdf::value(const Vec3F &dir) const
{
  const auto cos_theta = dot(dir.normalize(), onb_.w);
  assert(fabs(cos_theta) - 1. <= epsilon);
  return cos_theta <= 0 ? 0 : cos_theta / pi;
}

Vec3F CosinePdf::generate() const
{
  return onb_.local(cosine_direction_sample());
}
