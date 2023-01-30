#include "onb.hh"

using namespace rt;
using namespace gm;

Onb::Onb(Vec3F const &normal)
{
  w = normal.normalize();
  Vec3F dummy;
  if (fabs(w.x) > 0.9999) {
    dummy = Vec3F(0, 1, 0);
    // u = cross(w, dummy).normalize();
    // v = cross(w, u);
  } else {
    dummy = Vec3F(1, 0, 0);
    // v = cross(w, dummy).normalize();
    // u = cross(v, w);
  }
  
  // return;
#if 0
  dummy -= w * dot(dummy, w);
  u = dummy.normalize();
  v = cross(w, u);
#else
  v = cross(w, dummy).normalize();
  u = cross(w, v);
#endif
}
