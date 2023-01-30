#include "shape_list.hh"

#include <cassert>

#include "../accelerate/aabb.hh"
#include "../rt/hit_record.hh"
#include "../util/random.hh"

using namespace rt;
using namespace util;

bool ShapeList::hit(Ray const &ray, double tmin, double tmax,
                    HitRecord &record) const
{
  // 由于hit返回false时，record不一定没被修改，
  // 这样会得到覆盖原有的record，
  // 所以临时record是有必要的。
  HitRecord tmp_record;
  double cur_max = tmax;

  bool has_anything_hit = false;
  for (auto const &shape : shapes_) {
    if (shape->hit(ray, tmin, cur_max, tmp_record)) {
      has_anything_hit = true;
      record = std::move(tmp_record);
      cur_max = record.t;
    }
  }

  return has_anything_hit;
}

bool ShapeList::get_bounding_box(Aabb &output_box) const
{
  if (shapes_.empty()) return false;

  bool is_first_box = true;
  Aabb tmp_box;
  for (auto const &shape : shapes_) {
    if (!shape->get_bounding_box(tmp_box)) return false;

    output_box =
        is_first_box ? tmp_box : Aabb::surrouding_box(output_box, tmp_box);
  }
  return true;
}

double ShapeList::pdf_value(const Point3F &origin, const Vec3F &direction) const
{
  const double weight = 1. / shapes_.size();
  double ret = 0.;
  for (auto const &shape : shapes_)
    ret += weight * shape->pdf_value(origin, direction);
  return ret;
}

Vec3F ShapeList::random_direction(const Point3F &origin) const
{
  assert(!shapes_.empty());
  return shapes_[random_int(0, shapes_.size()-1)]->random_direction(origin);
}
