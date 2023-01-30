#ifndef RT_MIXTURE_PDF_HH__
#define RT_MIXTURE_PDF_HH__

#include "../shape/shape.hh"
#include "pdf.hh"

#include <vector>

namespace rt {

class MixturePdf : public Pdf {
 public:
  explicit MixturePdf(Pdf *p0, Pdf *p1)
  {
    pdfs_[0] = p0;
    pdfs_[1] = p1;
  }

  virtual double value(Vec3F const &dir) const;
  virtual Vec3F generate() const;

 private:
  Pdf* pdfs_[2];
};

} // namespace rt

#endif
