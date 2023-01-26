#include "box.hh"

#include "../accelerate/aabb.hh"
#include "rect.hh"

using namespace rt;
using namespace std;
using namespace gm;

Box::Box(Point3F const &bottom, Point3F const &top, MaterialSPtr const &material)
  : top_(top)
  , bottom_(bottom)
{
  faces_.add(make_shared<XyRect>(bottom.x, top.x, bottom.y, top.y, bottom.z, material));
  faces_.add(make_shared<XyRect>(bottom.x, top.x, bottom.y, top.y, top.z, material));
  faces_.add(make_shared<XzRect>(bottom.x, top.x, bottom.z, top.z, bottom.y, material));
  faces_.add(make_shared<XzRect>(bottom.x, top.x, bottom.z, top.z, top.y, material));
  faces_.add(make_shared<YzRect>(bottom.y, top.y, bottom.z, top.z, bottom.x, material));
  faces_.add(make_shared<YzRect>(bottom.y, top.y, bottom.z, top.z, top.x, material));
}

bool Box::hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const
{
  return faces_.hit(ray, tmin, tmax, record);
}

bool Box::get_bounding_box(Aabb &bbox) const
{
  bbox = Aabb(bottom_, top_);
  return true;
}
