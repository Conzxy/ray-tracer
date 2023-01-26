#ifndef RT_RAY_HH__
#define RT_RAY_HH__

#include <iosfwd>

#include "../gm/point.hh"
#include "../gm/vec.hh"

namespace rt {

class Ray {
 public:
  Ray() = default;
  Ray(gm::Point3F const &o, gm::Vec3F const &d)
    : o_(o)
    , d_(d)
  {
  }

  gm::Point3F origin() const noexcept { return o_; }
  gm::Vec3F direction() const noexcept { return d_; }

  gm::Point3F at(double t) const noexcept { return o_ + d_ * t; }

 private:
  gm::Point3F o_;
  gm::Vec3F d_;
};

std::ostream &operator<<(std::ostream &os, Ray const &ray);

} // namespace rt

#endif
