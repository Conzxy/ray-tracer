#include "rect.hh"

#include "../accelerate/aabb.hh"
#include "../rt/hit_record.hh"

using namespace rt;

bool XyRect::hit(Ray const &ray, double tmin, double tmax,
                 HitRecord &record) const
{
  auto t = (k_ - ray.origin().z) / ray.direction().z;
  if (t <= tmin || t >= tmax) return false;

  auto p = ray.at(t);
  if (p.x <= x1_ && p.x >= x0_ && p.y <= y1_ && p.y >= y0_) {
    record.p = p;
    record.t = t;
    record.material = material_.get();
    record.u = (p.x - x0_) / (x1_ - x0_);
    record.v = (p.y - y0_) / (y1_ - y0_);
    record.set_face_normal(ray, Vec3F(0, 0, 1));
    return true;
  }
  return false;
}

bool YzRect::hit(Ray const &ray, double tmin, double tmax,
                 HitRecord &record) const
{
  auto t = (k_ - ray.origin().x) / ray.direction().x;
  if (t <= tmin || t >= tmax) return false;

  auto p = ray.at(t);
  if (p.y <= y1_ && p.y >= y0_ && p.z <= z1_ && p.z >= z0_) {
    record.p = p;
    record.t = t;
    record.material = material_.get();
    record.u = (p.z - z0_) / (z1_ - z0_);
    record.v = (p.y - y0_) / (y1_ - y0_);
    record.set_face_normal(ray, Vec3F(1, 0, 0));
    return true;
  }
  return false;
}

bool XzRect::hit(Ray const &ray, double tmin, double tmax,
                 HitRecord &record) const
{
  auto t = (k_ - ray.origin().y) / ray.direction().y;
  if (t <= tmin || t >= tmax) return false;

  auto p = ray.at(t);
  if (p.x <= x1_ && p.x >= x0_ && p.z <= z1_ && p.z >= z0_) {
    record.p = p;
    record.t = t;
    record.material = material_.get();
    record.u = (p.x - x0_) / (x1_ - x0_);
    record.v = (p.z - z0_) / (z1_ - z0_);
    record.set_face_normal(ray, Vec3F(0, 1, 0));
    return true;
  }

  return false;
}

#define THICKNESS 0.001

bool XyRect::get_bounding_box(Aabb &bbox) const
{
  bbox = Aabb(Point3F(x0_, y0_, k_ - THICKNESS),
              Point3F(x1_, x1_, k_ + THICKNESS));
  return true;
}

bool YzRect::get_bounding_box(Aabb &bbox) const
{
  bbox = Aabb(Point3F(k_ - THICKNESS, y0_, z0_),
              Point3F(k_ + THICKNESS, y1_, z0_));
  return true;
}

bool XzRect::get_bounding_box(Aabb &bbox) const
{
  bbox = Aabb(Point3F(x0_, k_ - THICKNESS, z0_),
              Point3F(x1_, k_ + THICKNESS, z1_));
  return true;
}
