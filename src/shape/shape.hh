#ifndef SHAPE_HH__
#define SHAPE_HH__

#include <memory>

#include "../accelerate/aabb.hh"
#include "../rt/ray.hh"

namespace rt {

struct HitRecord;

class Shape
{
 public:
  virtual bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const = 0;
  virtual bool get_bounding_box(Aabb &output_box) const = 0;
  
  /**
   * \param origin The origin of the scatter ray
   * \param direction The direction of the scatter ray
   */
  virtual double pdf_value(gm::Point3F const &origin, gm::Vec3F const &direction) const
  {
    // FIXME Pure virtual function
    return 0;
  }

  /**
   * \param origin The origin of the scatter ray(i.e. the intersected point)
   */
  virtual gm::Vec3F random_direction(gm::Point3F const &origin) const
  {
    // FIXME Pure virtual function
    std::cout << "FIXME random_direction\n";
    return { 0, 0, 0 };
  }

  Aabb get_bounding_box() const;
};

using ShapePtr = std::shared_ptr<Shape>;
using ShapeSPtr = std::shared_ptr<Shape>;

} // namespace rt

#endif
