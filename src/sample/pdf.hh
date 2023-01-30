#ifndef RT_PDF_HH__
#define RT_PDF_HH__

#include <memory>

#include "../gm/vec.hh"

namespace rt {

class Pdf {
 public:
  virtual double value(Vec3F const &dir) const = 0;
  virtual Vec3F generate() const = 0;
};

using PdfSPtr = std::shared_ptr<Pdf>;

} // namespace rt

#endif