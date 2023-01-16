#include "gm/point.hh"

#include <gtest/gtest.h>

TEST (point_test, main) {
  gm::Point3F p(1, 2, 3);
  EXPECT_EQ(p.x, p[0]);
  EXPECT_EQ(p.y, p[1]);
  EXPECT_EQ(p.z, p[2]);
}
