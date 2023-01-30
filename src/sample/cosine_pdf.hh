#ifndef MATERIAL_COSINE_PDF_HH__
#define MATERIAL_COSINE_PDF_HH__

#include "pdf.hh"
#include "../gm/onb.hh"

namespace rt {

class CosinePdf : public Pdf {
 public:
  explicit CosinePdf(Vec3F const &normal);

  virtual double value(Vec3F const &dir) const;
  virtual Vec3F generate() const;
 private:
  gm::Onb onb_;
};

} // namespace rt

#endif
