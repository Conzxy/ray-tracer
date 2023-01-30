#ifndef SHAPE_RECT_HH__
#define SHAPE_RECT_HH__

#include "shape.hh"

#include <memory>

#include "../material/material.hh"

namespace rt {

class XyRect : public Shape {
 public:
  XyRect(double x0, double x1, double y0, double y1, double k,
         MaterialSPtr material)
    : x0_(x0)
    , x1_(x1)
    , y0_(y0)
    , y1_(y1)
    , k_(k)
    , material_(std::move(material))
  {
  }

  bool hit(Ray const &ray, double tmin, double tmax,
           HitRecord &record) const override;
  bool get_bounding_box(Aabb &bbox) const override;

  double width() const noexcept { return x1_ - x0_; }
  double height() const noexcept { return y1_ - y0_; }

 private:
  double x0_ = 0;
  double x1_ = 0;
  double y0_ = 0;
  double y1_ = 0;
  double k_;
  MaterialSPtr material_;
};

class YzRect : public Shape {
 public:
  YzRect(double y0, double y1, double z0, double z1, double k,
         MaterialSPtr material)
    : y0_(y0)
    , y1_(y1)
    , z0_(z0)
    , z1_(z1)
    , k_(k)
    , material_(std::move(material))
  {
  }

  bool hit(Ray const &ray, double tmin, double tmax,
           HitRecord &record) const override;
  bool get_bounding_box(Aabb &bbox) const override;

  double width() const noexcept { return z1_ - z0_; }
  double height() const noexcept { return y1_ - y0_; }

 private:
  double y0_, y1_, z0_, z1_, k_;
  MaterialSPtr material_;
};

class XzRect : public Shape {
 public:
  XzRect(double x0, double x1, double z0, double z1, double k,
         MaterialSPtr material)
    : x0_(x0)
    , x1_(x1)
    , z0_(z0)
    , z1_(z1)
    , k_(k)
    , material_(std::move(material))
  {
  }

  bool hit(Ray const &ray, double tmin, double tmax,
           HitRecord &record) const override;
  bool get_bounding_box(Aabb &bbox) const override;

  virtual double pdf_value(Point3F const &origin,
                           Vec3F const &direction) const override;
  virtual Vec3F random_direction(Point3F const &origin) const override;

  static std::shared_ptr<XzRect> create_based_mid(double x, double width,
                                                  double z, double height,
                                                  double k, MaterialSPtr mat)
  {
    return std::make_shared<XzRect>(x - width / 2, x + width / 2,
                                    z - height / 2, z + height / 2, k,
                                    std::move(mat));
  }

 private:
  double x0_, x1_, z0_, z1_, k_;
  MaterialSPtr material_;
};

} // namespace rt

#endif
