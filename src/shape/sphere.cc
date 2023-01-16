#include "sphere.hh"

#include <cassert>

#include "../rt/hit_record.hh"

using namespace rt;

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

    record.material = material_;
    record.t = root;
    record.p = ray.at(record.t);

    assert((record.p - center_).length() - radius_ <= 0.0001);
    record.set_face_normal(ray, normal(record.p));
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
