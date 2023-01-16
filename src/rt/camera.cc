#include "camera.hh"

#include "../gm/util.hh"

using namespace rt;
using namespace gm;

Camera::Camera(gm::Point3F lookfrom, gm::Point3F lookat, double aspect_ratio,
               double fov, double focus_dist, Vec3F up)
  : lookfrom_(lookfrom)
  , lookat_(lookat)
  , look_direction_(lookat_ - lookfrom_)
{
  // 这里没有必要乘以focus_dist，因为image plane是均一缩放的，
  // 光线的方向向量是不变的（尽管浮点数精度问题可能导致会有点偏移）。
  // 但如果这样做，后面的horizontal_和vertical_还是得乘上focus_dist，
  // 因为image plane平移了focus_dist
  // film_height_ = tan(gm::angle_to_radian(fov / 2)) * focus_dist * 2;
  film_height_ = tan(gm::angle_to_radian(fov / 2)) * 2;
  film_width_ = aspect_ratio * film_height_;

  x_axis_ = gm::cross(look_direction_, up).normalize();
  y_axis_ = gm::cross(x_axis_, look_direction_).normalize();

  horizontal_ = x_axis_ * film_width_ * focus_dist;
  vertical_ = y_axis_ * film_height_ * focus_dist;
  lower_left_corner_ =
      lookfrom + focus_dist * look_direction_.normalize() - horizontal_ / 2 - vertical_ / 2;
}

void Camera::set_aperture(double aperture) { len_radius_ = aperture / 2; }

Ray Camera::ray(double u, double v) const noexcept
{
  auto offset_corr = len_radius_ * random_in_unit_sphere();
  auto offset = offset_corr.x * x_axis_ + offset_corr.y * y_axis_;
  auto origin = lookfrom_ + offset;
  return Ray(origin,
             lower_left_corner_ + u * horizontal_ + v * vertical_ - origin);
}

void Camera::DebugPrint() const noexcept
{
  std::cout << "===== Camera information(start) =====\n"
            << "lookfrom: " << lookfrom_ << '\n'
            << "lookat: " << lookat_ << '\n'
            << "look direction: " << look_direction_ << '\n'
            << "film width: " << film_width_ << '\n'
            << "film height: " << film_height_ << '\n'
            << "aspect_ratio: " << film_width_ / film_height_ << '\n'
            << "horizontal: " << horizontal_ << '\n'
            << "vertical: " << vertical_ << '\n'
            << "lower_left_corner: " << lower_left_corner_ << '\n'
            << "focal length: " << focal_length_ << '\n'
            << "len_radius: " << len_radius_ << '\n'
            << "===== Camera information(end) =====\n";
}
