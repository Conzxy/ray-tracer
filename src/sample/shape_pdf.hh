#ifndef RT_SHAPE_PDF_HH__
#define RT_SHAPE_PDF_HH__

#include "pdf.hh"
#include "../shape/shape.hh"

namespace rt {

class ShapePdf : public Pdf {
 public:
  /**
   * \param origin The origin of onb(the intersected point normally)
   */
  ShapePdf(Shape *shape, Point3F const &origin)
    : shape_(shape)
    , origin_(origin)
  {
  }

  ShapePdf(ShapeSPtr const &shape, Point3F const &origin)
    : ShapePdf(shape.get(), origin)
  {
  }

  virtual double value(Vec3F const &dir) const override
  {
    return shape_->pdf_value(origin_, dir);
  }
  virtual Vec3F generate() const override
  {
    return shape_->random_direction(origin_);
  }
 private:
  Shape *shape_;
  gm::Point3F origin_;
};

}

#endif