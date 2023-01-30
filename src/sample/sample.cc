#include "sample.hh"

#include "../util/random.hh"

using namespace gm;
using namespace util;

namespace rt {

Vec3F cosine_direction_sample()
{
  const auto r1 = random_double();
  const auto r2 = random_double();
  const auto sqrt_r2 = sqrt(r2);

  auto phi = 2 * pi * r1;
  return {
      cos(phi) * sqrt_r2,
      sin(phi) * sqrt_r2,
      sqrt(1 - r2),
  };
}

Vec3F sphere_direction_sample(double radius, double distance_squared)
{
  const auto r1 = random_double();
  const auto r2 = random_double();
  auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
  const auto phi = 2 * pi * r1;
  auto sin_theta = sqrt(1 - z * z);
  const auto x = cos(phi) * sin_theta;
  const auto y = sin(phi) * sin_theta;
  // 此时，center_ - origin就是旋转轴(局部坐标系的z轴)
  return {x, y, z};
}

} // namespace rt
