#include "translate.hh"

#include "../rt/hit_record.hh"
#include "../accelerate/aabb.hh"

using namespace rt;

bool Translate::hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const
{
  Ray moved_ray(ray.origin()-offset_, ray.direction());

  auto ret = shape_->hit(moved_ray, tmin, tmax, record);
  if (!ret) return false;

  record.p += offset_;
  record.set_face_normal(moved_ray, record.normal);
  return true;
}

bool Translate::get_bounding_box(Aabb &bbox) const
{
  if (!shape_->get_bounding_box(bbox)) {
    return false;
  }

  bbox = Aabb(bbox.min() + offset_, bbox.max() + offset_);
  return true;
}
