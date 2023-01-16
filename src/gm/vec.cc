#include "vec.hh"

using namespace gm;

namespace gm {

Vec3F random_in_unit_sphere()
{
  Vec3F ret;
  do {
    ret = Vec3F::random(-1, 1); 
  } while (ret.length_squared() >= 1.);

  return ret;
}

Vec3F random_on_unit_sphere_surface()
{
  Vec3F ret = Vec3F::random();
  return ret.normalize();
}

}
