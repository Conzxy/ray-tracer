#include "bvh_node.hh"

#include <algorithm>
#include <cassert>
#include <cstdio>

#include "../rt/hit_record.hh"
#include "../shape/shape.hh"
#include "../util/random.hh"

using namespace util;

namespace rt {

static bool shape_box_compare(ShapeSPtr const &lhs, ShapeSPtr const &rhs,
                              int i) noexcept
{
  Aabb box0;
  Aabb box1;

  if (!lhs->get_bounding_box(box0) || !rhs->get_bounding_box(box1)) {
    fprintf(stderr, "No bounding box can be used for compareing!\n");
    abort();
  }
  return box0.min()[i] < box1.min()[i];
}

inline static bool shape_box_x_compare(ShapeSPtr const &lhs,
                                       ShapeSPtr const &rhs) noexcept
{
  return shape_box_compare(lhs, rhs, 0);
}

inline static bool shape_box_y_compare(ShapeSPtr const &lhs,
                                       ShapeSPtr const &rhs) noexcept
{
  return shape_box_compare(lhs, rhs, 1);
}

inline static bool shape_box_z_compare(ShapeSPtr const &lhs,
                                       ShapeSPtr const &rhs) noexcept
{
  return shape_box_compare(lhs, rhs, 2);
}

using CompareFunc = bool (*)(ShapeSPtr const &, ShapeSPtr const &);

static CompareFunc get_axis_comparator(int index) noexcept
{
  switch (index) {
    case 0:
      return shape_box_x_compare;
    case 1:
      return shape_box_y_compare;
    case 2:
      return shape_box_z_compare;
  }
  return nullptr;
}

static bool bvh_is_leaf(BvhNode const *node) noexcept
{
  return node->shape != nullptr;
}

static BvhNode *
build_bvh_subtree(std::vector<std::shared_ptr<Shape>> const &objects,
                  size_t start, size_t end)
{
  auto scene = objects; // for std::sort()/std::partial_sort()

  int axis_index = random_int(0, 2);
  CompareFunc comparator = get_axis_comparator(axis_index);
  assert(comparator);

  auto obj_sz = end - start;
  BvhNode *node = new BvhNode;
  if (obj_sz == 1) {
    node->right = node->left = new BvhNode(scene[start]);
  } else if (obj_sz == 2) {
    if (comparator(scene[start], scene[start+1])) {
      node->left = new BvhNode(std::move(scene[start]));
      node->right = new BvhNode(std::move(scene[start+1]));
    } else {
      node->left = new BvhNode(std::move(scene[start+1]));
      node->right = new BvhNode(std::move(scene[start]));
    }
  } else {
    auto mid = start + obj_sz / 2;
    std::partial_sort(scene.begin()+start, scene.begin()+mid, scene.begin()+end, comparator);
    node->left = build_bvh_subtree(objects, start, mid);
    node->right = build_bvh_subtree(objects, mid, end);
  }

  Aabb bbox0;
  Aabb bbox1;
  if (!node->left->get_bounding_box(bbox0) ||
      !node->right->get_bounding_box(bbox1))
  {
    fprintf(stderr,
            "The left or right node of the BvhNode is no bounding box!\n");
    abort();
  }

  node->box = Aabb::surrouding_box(bbox0, bbox1);
  return node;
}

BvhNode *build_bvh_tree(std::vector<std::shared_ptr<Shape>> const &objects)
{
  return build_bvh_subtree(objects, 0, objects.size());
}

bool BvhNode::hit(Ray const &ray, double tmin, double tmax,
                  HitRecord &record) const
{
  if (!box.hit(ray, tmin, tmax)) return false;

  if (shape) return shape->hit(ray, tmin, tmax, record);
  
  auto is_left_hit = left->hit(ray, tmin, tmax, record);
  auto is_right_hit =
      right->hit(ray, tmin, is_left_hit ? record.t : tmax, record);
  
  return is_left_hit || is_right_hit;
}

bool BvhNode::get_bounding_box(Aabb &bbox) const
{
  if (shape) return shape->get_bounding_box(bbox);
  bbox = box;
  return true;
}

void destroy_bvh_tree(BvhNode *root) noexcept
{
  if (root->left->shape && root->right->shape) {
    delete root;
    return;
  }
  destroy_bvh_tree(root->left);
  destroy_bvh_tree(root->right);
}

static void print_bvhtree(std::ostream &os, BvhNode const *node)
{
  Aabb box = node->box;
  os << "bbox: " << box.min() << ", " << box.max() << "\n";
  os << node << " is ";
  if (node->shape) {
    os << "shape\n";
    return;
  }
  else
    os << "bvh node\n";
  os << "left: "; 
  print_bvhtree(os, node->left);
  os << "right: ";
  print_bvhtree(os, node->right);
}

std::ostream &operator<<(std::ostream &os, BvhTree const &tree)
{
  print_bvhtree(os, tree.root());
  return os;
}

} // namespace rt
