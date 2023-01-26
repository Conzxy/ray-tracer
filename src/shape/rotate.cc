#include "rotate.hh"
#include "../gm/transform.hh"
#include "../rt/hit_record.hh"

using namespace rt;
using namespace gm;

Rotate::Rotate(ShapeSPtr shape, Degree const &degree)
  : shape_(shape)
  , reverse_rotate_mat_(get_x_rotation_matrix(-degree.x) *
                        get_y_rotation_matrix(-degree.y) *
                        get_z_rotation_matrix(-degree.z))
  , rotate_mat_(get_x_rotation_matrix(degree.x) *
                get_y_rotation_matrix(degree.y) *
                get_z_rotation_matrix(degree.z))
{
  has_bbox_ = shape_->get_bounding_box(cache_bbox_);
  if (has_bbox_) {
    cache_bbox_ = Aabb(rotate_mat_ * cache_bbox_.min(),
                       rotate_mat_ * cache_bbox_.max());
  }
}

bool Rotate::hit(const Ray &ray, double tmin, double tmax,
                 HitRecord &record) const
{
  Ray r_ray(reverse_rotate_mat_ * ray.origin(), reverse_rotate_mat_ * ray.direction());
  if (!shape_->hit(r_ray, tmin, tmax, record))
    return false;

  record.p = rotate_mat_ * record.p;
  record.set_face_normal(r_ray, rotate_mat_ * record.normal);
  return true;
}

bool Rotate::get_bounding_box(Aabb &output_box) const
{
  if (has_bbox_) {
    output_box = cache_bbox_;
  }
  return has_bbox_;
}