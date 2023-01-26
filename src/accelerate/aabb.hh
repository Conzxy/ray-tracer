#ifndef ACCELERATE_AABB_HH__
#define ACCELERATE_AABB_HH__

#include "../gm/point.hh"

namespace rt {

class Ray;

class Aabb {
 public:
  Aabb() = default;
  Aabb(gm::Point3F const &minimum, gm::Point3F const &maximum) noexcept
    : minimum_(minimum)
    , maximum_(maximum)
  {
  }

  gm::Point3F min() const noexcept { return minimum_; }
  gm::Point3F max() const noexcept { return maximum_; }

  bool hit(Ray const &r, double tmin, double tmax) const;

  static Aabb surrouding_box(Aabb const &box0, Aabb const &box1) noexcept;

  int get_longest_axis_index() const noexcept;
 private:
  gm::Point3F minimum_;
  gm::Point3F maximum_;
};

} // namespace rt

#endif
