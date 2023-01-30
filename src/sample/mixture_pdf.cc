#include "mixture_pdf.hh"

#include <algorithm>

using namespace rt;
using namespace util;

double MixturePdf::value(const Vec3F &dir) const
{
  return 0.5 * pdfs_[0]->value(dir) + 0.5 * pdfs_[1]->value(dir);
}

Vec3F MixturePdf::generate() const
{
  if (random_double() < 0.5)
    return pdfs_[0]->generate();
  return pdfs_[1]->generate();
}
