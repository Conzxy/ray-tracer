#ifndef RT_HIT_RECORD_HH__
#define RT_HIT_RECORD_HH__

#include "../gm/point.hh"
#include "../gm/vec.hh"
#include "ray.hh"

namespace rt {

class Material;

struct HitRecord {
  // 用于更新t的边界值，获取最近的t
  double t;
  // 用于光线反射，折射等计算
  Vec3F normal;
  // 射线的终点和新射线的起点
  Point3F p;

  // 折射Snell's Law涉及两边介质的比值。
  // 如果是normal被反转了，介质比值也得反转。
  // 因此需要记录。
  bool front_face;

  Material *material;

  void set_face_normal(Ray const &r, Vec3F const &outward_normal) noexcept
  {
    // 我们想要法向量朝向camera，不然影响后面的计算
    // 当射线与给定的法向量点乘<0时，表示法向量朝向camera，否则反转给定法向量，这样，法向量必然朝向camera
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

} // namespace rt

#endif
