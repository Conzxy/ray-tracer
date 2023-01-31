#include "constant_medium.hh"

#include "../material/iostropic.hh"
#include "../texture/solid_texture.hh"
#include "../rt/hit_record.hh"

using namespace rt;
using namespace std;
using namespace gm;
using namespace util;

ConstantMedium::ConstantMedium(ShapeSPtr &&boundary, double density,
                               TextureSPtr albedo)
  : boundary_(std::move(boundary))
  , density_(density)
  , phase_function_(make_shared<Iostropic>(albedo))
{
}

ConstantMedium::ConstantMedium(ShapeSPtr &&boundary, double density, Color albedo)
  : ConstantMedium(std::move(boundary), density, make_shared<SolidTexture>(albedo))
{
}

bool ConstantMedium::hit(Ray const &ray, double tmin, double tmax,
                         HitRecord &record) const
{
  // 先获取boundary的两个交点上下文
  HitRecord rec1, rec2;
  if (!boundary_->hit(ray, -inf, inf, rec1)) return false;
  if (!boundary_->hit(ray, rec1.t+0.0001, inf, rec2)) return false;
  
  // volume的第一个交点在tmin之后，比如ray.origin后面，tmin之后的距离舍去
  // 处理ray.origin在volume内的情形
  rec1.t = std::max(rec1.t, tmin);
  // volume的第二个交点在tmax之前，说明光线被其他物体遮挡，tmax之前的距离舍去
  rec2.t = std::min(rec2.t, tmax);
  
  if (rec1.t >= rec2.t) return false;

  // 可能是由于ray.origin在volume中，计算得到的t由于精度问题导致是个接近0的负数，需要矫正为0？
  // 除非tmin < 0
  if (rec1.t < 0) rec1.t = 0;
  const auto ray_distance = ray.direction().length();
  const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_distance;
  const auto hit_distance = -(1 / density_) * log(random_double());
  // 当在boundary中的前进距离大于某个值时，发生散射
  if (hit_distance > distance_inside_boundary) return false;

  record.t = rec1.t + hit_distance / ray_distance;
  record.p = ray.at(record.t);
  record.material = phase_function_.get();
  record.normal = Vec3F(1, 0, 0); // 随意
  record.front_face = true; // 随意

  return true;
}

bool ConstantMedium::get_bounding_box(Aabb &output_box) const
{
  return boundary_->get_bounding_box(output_box);
}
