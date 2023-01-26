#include "shape.hh"
#include "../accelerate/aabb.hh"

using namespace rt;

Aabb Shape::get_bounding_box() const
{
  Aabb bbox;
  get_bounding_box(bbox);
  return bbox;
}
