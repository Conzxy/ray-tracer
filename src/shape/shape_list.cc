#include "shape_list.hh"

#include "../accelerate/aabb.hh"
#include "../rt/hit_record.hh"

using namespace rt;

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

