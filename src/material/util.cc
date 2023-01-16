#include "util.hh"

using namespace gm;

namespace rt {

Vec3F reflect(Vec3F const &v, Vec3F const &n)
{
  /*
   * v: incident light
   * n: surface normal
   *
   * u =
   * v + 2 * n * |v| * cos< -v, n> =
   * v + 2 * n * dot(-v, n)
   * v - 2 * n * dot(v, n)
   */
  return v - 2 * dot(v, n) * n;
}

Vec3F refract(Vec3F const &r, Vec3F const &n, double eta_over_eta2)
{
  // 浮点数精度问题 ==> fmin()保证在1.0以内
  auto cos_theta = fmin(dot(-r, n), 1.0);
  auto out_r_x = eta_over_eta2 * (r + cos_theta * n);
  auto out_r_y = -sqrt(1-out_r_x.length_squared()) * n;
  return out_r_x + out_r_y;
}

}
