#include "gm/vec.hh"

#include <gtest/gtest.h>

using namespace gm;


TEST (vec_test, main) {
  gm::Vec3i v(1, 2, 3);
  EXPECT_EQ(v.x, v[0]);
  EXPECT_EQ(v.y, v[1]);
  EXPECT_EQ(v.z, v[2]);
}

TEST (vec_test, equal) {
  gm::Vec3F v(0.5, 0.5, 0.5);
  EXPECT_NE(v, Vec3F(0, 0, 0));
}