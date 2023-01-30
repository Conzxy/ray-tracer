#ifndef RT_MATERIAL_ONB_HH__
#define RT_MATERIAL_ONB_HH__

#include "../gm/vec.hh"

namespace gm {

class Onb {
 public:
  gm::Vec3F u;
  gm::Vec3F v;
  gm::Vec3F w;

  explicit Onb(gm::Vec3F const &normal);

  gm::Vec3F local(double x, double y, double z) const noexcept
  {
    return u * x + v * y + w * z;
  }

  gm::Vec3F local(gm::Vec3F const &v) const noexcept
  {
    return local(v.x, v.y, v.z);
  }

};

}

#endif
