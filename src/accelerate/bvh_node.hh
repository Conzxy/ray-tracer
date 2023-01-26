#ifndef ACCELERATE_BVH_NODE_HH__
#define ACCELERATE_BVH_NODE_HH__

#include <memory>
#include <vector>
#include <iosfwd>

#include "../shape/shape.hh"
#include "aabb.hh"

namespace rt {

class Shape;

/**
 * FIXME 是否继承Shape?
 * 在The next week中作者是继承了Shape接口类, 因为从结果来看, 最终我们
 * 终究是调用叶子节点(即Shape的实现类)的hit()等接口函数.
 */
struct BvhNode {
  BvhNode *left = nullptr;
  BvhNode *right = nullptr;
  ShapeSPtr shape = nullptr;
  Aabb box{};
  
  BvhNode() = default;

  explicit BvhNode(ShapeSPtr shape_)
    : shape(std::move(shape_))
    , box(shape->get_bounding_box())
  {
  }
  
  ~BvhNode() noexcept
  {

  }
  bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const;

  bool get_bounding_box(Aabb &bbox) const;
};

BvhNode *build_bvh_tree(std::vector<std::shared_ptr<Shape>> const &objects);
void destroy_bvh_tree(BvhNode *root) noexcept;

class BvhTree : public Shape
{
 public:
  explicit BvhTree(std::vector<std::shared_ptr<Shape>> const &objects)
  {
    root_ = build_bvh_tree(objects);
  }

  ~BvhTree() noexcept { destroy_bvh_tree(root_); }
  

  bool hit(Ray const &ray, double tmin, double tmax, HitRecord &record) const override
  {
    return root_->hit(ray, tmin, tmax, record);
  }

  bool get_bounding_box(Aabb &output_box) const override
  {
    return root_->get_bounding_box(output_box);
  }

  BvhNode const *root() const noexcept { return root_; }
  
  friend std::ostream &operator<<(std::ostream &os, BvhTree const &tree);
 private:
  BvhNode *root_;
};

} // namespace rt

#endif
