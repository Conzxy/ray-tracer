#include "aabb.hh"

#include "../rt/ray.hh"

using namespace rt;

bool Aabb::hit(Ray const &r, double tmin, double tmax) const
{
  double t0, t1;
  for (int i = 0; i < 3; ++i) {
    auto tmp_t0 = (minimum_[i] - r.origin()[i]) / r.direction()[i];
    auto tmp_t1 = (maximum_[i] - r.origin()[i]) / r.direction()[i];
    t0 = fmin(tmp_t0, tmp_t1);
    t1 = fmax(tmp_t0, tmp_t1);

    tmin = fmax(t0, tmin);
    tmax = fmin(t1, tmax);
    if (tmax <= tmin) return false;
  }
  return true;
}

Aabb Aabb::surrouding_box(Aabb const &box0, Aabb const &box1) noexcept
{
  return {{
              fmin(box0.min().x, box1.min().x),
              fmin(box0.min().y, box1.min().y),
              fmin(box0.min().z, box1.min().z),
          },
          {
              fmax(box0.max().x, box1.max().x),
              fmax(box0.max().y, box1.max().y),
              fmax(box0.max().z, box1.max().z),
          }};
}

int Aabb::get_longest_axis_index() const noexcept
{
  auto delta_x = maximum_.x - minimum_.x;
  auto delta_y = maximum_.y - maximum_.y;
  auto delta_z = maximum_.z - maximum_.z;

  return (delta_x < delta_y) ? ((delta_y < delta_z) ? 2 : 1)
                             : ((delta_x < delta_z) ? 2 : 0);
}
