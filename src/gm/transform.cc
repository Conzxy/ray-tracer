#include "transform.hh"

#include <cmath>

namespace gm {

Matrix3x3F get_x_rotation_matrix(double angle)
{
  const auto rad = angle_to_radian(angle);
  const auto sin_theta = sin(rad);
  const auto cos_theta = cos(rad);
  return {
      {1, 0, 0},
      {0, cos_theta, -sin_theta},
      {0, sin_theta, cos_theta},
  };
}

Matrix3x3F get_y_rotation_matrix(double angle)
{
  const auto rad = angle_to_radian(angle);
  const auto sin_theta = sin(rad);
  const auto cos_theta = cos(rad);
  return {
      {cos_theta, 0, sin_theta},
      {0, 1, 0},
      {-sin_theta, 0, cos_theta},
  };
}

Matrix3x3F get_z_rotation_matrix(double angle)
{
  const auto rad = angle_to_radian(angle);
  const auto sin_theta = sin(rad);
  const auto cos_theta = cos(rad);
  return {
      {cos_theta, -sin_theta, 0},
      {sin_theta, cos_theta, 0},
      {0, 0, 1},
  };
}

} // namespace gm