#ifndef GM_UTIL_HH__
#define GM_UTIL_HH__

#include <limits>
#include <cmath>

namespace gm {

constexpr double pi = 3.141592654;
constexpr double inf = std::numeric_limits<double>::infinity();
constexpr double epsilon = 1e-8;

inline double angle_to_radian(double angle) noexcept
{
  return angle * pi / 180;
}

inline double lerp(double start, double end, double t) noexcept
{
  return (1-t) * start + t * end;
}

inline double clamp(double x, double bottom, double top) noexcept
{
  return (x < bottom) ? bottom : ((x > top) ? top : x);
}

inline double sin_from_cos(double x) noexcept
{
  return sqrt(1.0 - x * x);
}

} // namespace gm

#endif