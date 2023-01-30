#include "sphere.hh"

#include <cassert>

#include "../rt/hit_record.hh"
#include "../accelerate/aabb.hh"
#include "../gm/onb.hh"
#include "../sample/sample.hh"

using namespace rt;
using namespace util;
using namespace gm;

bool Sphere::hit(Ray const &ray, double tmin, double tmax,
                 HitRecord &record) const
{
  auto co = ray.origin() - center_;
  auto a = ray.direction().length_squared();
  auto half_b = dot(ray.direction(), co);
  auto c = co.length_squared() - radius_ * radius_;

  auto delta = half_b * half_b - a * c;

  if (delta > 0) {
    auto sqrt_delta = std::sqrt(delta);
    auto root = (-half_b - sqrt_delta) / a;
    if (root < tmin || tmax < root) {
      root = (-half_b + sqrt_delta) / a;
      if (root < tmin || tmax < root) {
        return false;
      }
    }

    record.material = material_.get();
    record.t = root;
    record.p = ray.at(record.t);

    assert((record.p - center_).length() - radius_ <= 0.0001);
    auto outward_normal = normal(record.p);
    record.set_face_normal(ray, outward_normal);
    // outward normal is also the point in the identity sphere
    get_uv(outward_normal, record.u, record.v);
    return true;
  }

  return false;
}

Vec3F Sphere::normal(Point3F const &p) const noexcept
{
  // 法向量单位化是有必要的，
  // 这样点乘可以表示夹角余弦值
  return (p - center_) / radius_;
}

bool Sphere::get_bounding_box(Aabb &output_box) const
{
  output_box = Aabb(center_ - radius_, center_ + radius_);
  return true;
}

void Sphere::get_uv(Point3F const &p, double &u, double &v)
{
  // radius == 1
  auto theta = acos(-p.y);
  auto pi = atan2(-p.z, p.x) + gm::pi;
  u = pi / (gm::pi * 2);
  v = theta / gm::pi;
}

double Sphere::pdf_value(const Point3F &origin, const Vec3F &direction) const
{
  HitRecord rec;
  if (!Sphere::hit(Ray(origin, direction), 0.001, inf, rec)) return 0;
  auto cos_theta_max = sqrt(1 - radius_ * radius_ / (center_ - origin).length_squared());
  auto solid_angle = 2 * pi * (1 - cos_theta_max);
  return 1 / solid_angle;
}

Vec3F Sphere::random_direction(const Point3F &origin) const
{
  auto z_axis = center_ - origin;
  Onb onb(z_axis);
  return onb.local(sphere_direction_sample(radius_, z_axis.length_squared()));
}
