#ifndef RT_CAMERA_HH__
#define RT_CAMERA_HH__

#include "../gm/point.hh"
#include "../gm/util.hh"
#include "../gm/vec.hh"
#include "../rt/ray.hh"

namespace rt {

class Camera {
 public:
  Camera(gm::Point3F lookfrom, gm::Point3F lookat, double aspect_ratio,
         double fov = 90, double focus_dist = 1, Vec3F up = Vec3F(0., 1., 0.));
  Ray ray(double u, double v) const noexcept;

  void set_aperture(double aperture);

  void DebugPrint() const noexcept;

 private:
  gm::Point3F lookfrom_;
  gm::Point3F lookat_;
  gm::Vec3F look_direction_;

  gm::Vec3F horizontal_;
  gm::Vec3F vertical_;

  gm::Point3F lower_left_corner_;
  // Film
  double focal_length_ = 1.0;

  double film_height_;
  double film_width_;

  Vec3F x_axis_;
  Vec3F y_axis_;
  double len_radius_ = 0;
};

} // namespace rt

#endif
