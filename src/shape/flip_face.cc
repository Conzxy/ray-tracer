#include "flip_face.hh"

#include "../rt/hit_record.hh"

using namespace rt;

bool FlipFace::hit(Ray const &ray, double tmin, double tmax, HitRecord &rec) const
{
  if (!shape_->hit(ray, tmin, tmax, rec)) return false;
  // FIXME flip normal
  rec.front_face = !rec.front_face;
  return true;
}

bool FlipFace::get_bounding_box(Aabb &bbox) const
{
  if (!shape_->get_bounding_box(bbox)) return false;
  return true;
}